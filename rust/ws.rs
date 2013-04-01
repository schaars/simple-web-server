/*
 * Simple web server in Rust.
 * For now, the pool_size argument is not used: there is at most only
 * one client at a time.
 */
extern mod std;
use std::getopts::*;
use std::net::tcp;
use std::net::ip;
use std::task;
use std::uv;
use pipes::{stream, Port, Chan};

type ConnectMsg = (tcp::TcpNewConnection, core::oldcomm::Chan<Option<tcp::TcpErrData>>);

fn print_usage(program: ~str) {
   fail fmt!("Usage: %s -p port -s pool_size -d web_dir", program);
}

// Another version to parse the arguments, which uses getopts
fn parse_arguments_with_getopts(args: ~[~str]) -> (uint, uint, ~str) {
   let opts = ~[
      optopt("p"),
      optopt("s"),
      optopt("d")
   ];
   let matches = match getopts(vec::tail(args), opts) {
      result::Ok(m) => { m }
      result::Err(f) => { fail fail_str(f) }
   };
   let port = uint::from_str(opt_str(&matches, "p")).get();
   let pool_size = uint::from_str(opt_str(&matches, "s")).get();
   let web_dir = opt_str(&matches, "d");

   if port == -1 || pool_size == -1 || web_dir == ~"" {
      io::println("Bad arguments values: ");
      print_usage(copy args[0]);
   }

   (port, pool_size, web_dir)
}

/* read the file path */
fn read_file(path: ~str) -> (~[u8], int) {
   let res = io::read_whole_file(&Path(path));
   if result::is_err(&res) {
      // XXX how to make the difference between file not found and not authorized?
      // In both cases res = Err(~"error opening file")
      (~[], 404)
   } else {
      (res.get(), 200)
   }
}

/*
 * Read an HTTP request and return a couple (line, code).
 * Block until a line has been read, in which case line is
 * the line that has been read and code=0, or return the line
 * that has been read so far and code=1 in case of error.
 */
fn read_request(socket: &tcp::TcpSocket) -> (~str, uint) {
   let mut req: ~str = ~"";
   loop {
      let result = socket.read(0u);
      if result.is_err() {
         return (req, 1);
      }

      let req2 = req + str::from_bytes(result.get());
      req = move req2;
      if str::contains(req, "\n") {
         return (req, 0);
      }
   }
}

fn clienterror(socket: &tcp::TcpSocket, cause: ~str, errnum: @str, shortmsg: @str, longmsg: @str) {
   io::println(fmt!("Client error: %s -> %s %s %s", cause, errnum, shortmsg, longmsg));

   let body = ~"<html><title>Simple Web Server Error</title><body bgcolor=\"ffffff\">\r\n"
      + fmt!("%s: %s\r\n", errnum, shortmsg)
      + fmt!("<p>%s: %s</p>\r\n", longmsg, cause)
      + ~"<hr><em>The Simple Web Server</em>\r\n"; 
   let header = fmt!("HTTP/1.0 %s %s\r\n", errnum, shortmsg) + ~"Content-type: text/html\r\n" + fmt!("Content-length: %u\r\n\r\n", body.len());

   socket.write(str::to_bytes(header));
   socket.write(str::to_bytes(body));
}

fn get_file_type(filename: ~str) -> ~str {
   if str::ends_with(filename, ~".html") {
      ~"text/html"
   } else if str::ends_with(filename, ~".gif") {
      ~"image/gif"
   } else if str::ends_with(filename, ~".jpg") {
      ~"image/jpeg"
   } else {
      ~"text/plain"
   }
}

fn clientsuccess(socket: &tcp::TcpSocket, filename: ~str, content: ~[u8]) {
   let filetype = get_file_type(filename);

   let header = ~"HTTP/1.0 200 OK\r\nServer: Simple Web Server\r\n"
      +fmt!("Content-length: %u\r\n", content.len())
      +fmt!("Content-type: %s\r\n\r\n", filetype);

   socket.write(str::to_bytes(header));
   socket.write(content);
}

fn handle_connection(port_endpoint: ~Port<ConnectMsg>, web_dir: ~str) {
   // this is the task which accepts new connections
   do task::spawn{
      loop {
         let (conn, kill_ch) = port_endpoint.recv();
         io::println("Going to accept a new connection");
         match tcp::accept(conn) {
            result::Err(err) => {
               io::println(fmt!("Connection error: %?", err));
               kill_ch.send(Some(err));
            },
            result::Ok(socket) => {
               let peer_addr: ~str = ip::format_addr(&socket.get_peer_addr());
               io::println(fmt!("Connection accepted from %s", peer_addr));

               let (request, code) = read_request(&socket);
               if code == 0 {
                  let words = str::words(request);
                  if words.len() < 3 {
                     clienterror(&socket, request, @"400", @"Bad request", @"The request is not well-formatted");
                  } else if words[0] != ~"GET" {
                     clienterror(&socket, request, @"501", @"Not implemented", @"The request is not implemented");
                  } else if words[2] != ~"HTTP/1.0" {
                     clienterror(&socket, request, @"505", @"HTTP version not supported", @"The server does not implement this version of HTTP");
                  } else {
                     let (content, code) = read_file(web_dir + words[1]);
                     if code == 200 {
                        clientsuccess(&socket, copy words[1], content);
                     } else if code == 404 {
                        clienterror(&socket, copy words[1], @"404", @"Not found", @"The file cannot be found");
                     } else if code == 401 {
                        clienterror(&socket, copy words[1], @"401", @"Not authorized", @"The file cannot be accessed");
                     } else {
                        fail fmt!("Unknown error code: %d", code);
                     }
                  }
               }
            }
         }
      }
   }
}

fn main()  {
   let args : ~[~str] = os::args();
   if (args.len() != 7) {
      io::print("Bad length: ");
      print_usage(copy args[0]);
   }

   let (port, pool_size, web_dir) = parse_arguments_with_getopts(args);

   //to play with string concatenation
   io::println(~"port is " + uint::to_str(port, 10));
   io::println(~"pool size is " + uint::to_str(pool_size, 10));
   io::println(~"web dir is " + web_dir);

   //Connection information will be transmitted using this Port and Chan
   let (port_endpoint, chan_endpoint): (Port<ConnectMsg>, Chan<ConnectMsg>) = stream();

   handle_connection(~port_endpoint, web_dir);

   let result = tcp::listen(ip::v4::parse_addr("127.0.0.1"), port, 100, 
         uv::global_loop::get(),
         |_|{
            io::println("Server is now listening!");
         },
         |conn, kill_chan| {
            // The connection must be accepted from another task or the server will block.
            chan_endpoint.send((conn, kill_chan));
         }
   );

   if result.is_err() {
      fail fmt!("failed listen: %?", result.get_err());
   }
}

