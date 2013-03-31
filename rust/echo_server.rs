/* 
 * A simple echo server in TCP: accepts connections, read and write lines
 * Useful links:
 *   -https://github.com/mozilla/rust/blob/incoming/src/libstd/flatpipes.rs#L772
 *   -https://github.com/mozilla/rust/issues/4296
 *   -https://gist.github.com/thomaslee/4753338
 */

/*
 * TODO: does not work yet: the server blocks after "server is now listening".
 * When a client (e.g., telnet) connects, the message "New client" is not displayed
 */

extern mod std;
use std::net::tcp;
use std::net::ip;
use std::uv;
use core::pipes;

fn new_connect_cb(new_client: tcp::TcpNewConnection, _comm_chan: core::oldcomm::Chan<core::option::Option<std::net_tcp::TcpErrData>>) {
   do task::spawn {
      error!("New client");
      error!("going to accept");
      let result = tcp::accept(new_client);
      error!("ok for the call");
      if result.is_ok(){
         error!("Accepted!");
         let socket = result::unwrap(move result);
         error!("Unwrapped");
         // Now do stuff with this socket
         let data = socket.read(1); // XXX: This blocks
         io::println(fmt!("%?", data));
      }else{
         error!("Not accepted!");
      }
   }
}

fn main() {
   // We need to create several tasks
   // 1 to listen, 1 to accept
   // We need a channel between the tasks

   do task::spawn_sched(task::ManualThreads(1)) {
      let result = tcp::listen(ip::v4::parse_addr("127.0.0.1"), 4000, 100, 
            uv::global_loop::get(),
            |_comm_chan|{
            io::println("Server is now listening!");
            },
            new_connect_cb
      );
   
      if result.is_err() {
         fail fmt!("failed listen: %?", result.get_err());
      }
   }
}
