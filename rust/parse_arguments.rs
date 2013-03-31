/*
 * How to get and parse the arguments on the command line 
 */
extern mod std;
use std::getopts::*;

fn print_usage(program: ~str) {
   fail fmt!("Usage: %s -p port -s pool_size -d web_dir", program);
}

// My personal version of the parsing of arguments. We iterate over
// args to find the needed ones.
fn parse_arguments_with_iteration(args: ~[~str]) -> (int, int, ~str) {
   let mut port: int = -1;
   let mut pool_size: int = -1;
   let mut web_dir: ~str = ~"";
   let mut i = 1;

   while i<args.len() {
      match args[i] {
         ~"-p" =>  { port = int::from_str(args[i+1]).get();  i+=1}
         ~"-s" =>  { pool_size = int::from_str(args[i+1]).get();  i+=1}
         ~"-d" =>  { web_dir = copy args[i+1]; i+=1}
         _ => io::println(fmt!("Argument %s is unknown", args[i]))
      }
      i+=1;
   }

   if port == -1 || pool_size == -1 || web_dir == ~"" {
      io::println("Bad arguments values: ");
      print_usage(copy args[0]);
   }

   (port, pool_size, web_dir)
}

// Another version to parse the arguments, which uses getopts
fn parse_arguments_with_getopts(args: ~[~str]) -> (int, int, ~str) {
   let opts = ~[
      optopt("p"),
      optopt("s"),
      optopt("d")
   ];
   let matches = match getopts(vec::tail(args), opts) {
      result::Ok(m) => { m }
      result::Err(f) => { fail fail_str(f) }
   };
   let port = int::from_str(opt_str(&matches, "p")).get();
   let pool_size = int::from_str(opt_str(&matches, "s")).get();
   let web_dir = opt_str(&matches, "d");

   if port == -1 || pool_size == -1 || web_dir == ~"" {
      io::println("Bad arguments values: ");
      print_usage(copy args[0]);
   }

   (port, pool_size, web_dir)
}

fn main()  {
   let args : ~[~str] = os::args();
   if (args.len() != 7) {
      io::print("Bad length: ");
      print_usage(copy args[0]);
   }

   //let (port, pool_size, web_dir) = parse_arguments_with_iteration(args);
   let (port, pool_size, web_dir) = parse_arguments_with_getopts(args);

   //to play with string concatenation
   io::println(~"port is " + int::to_str(port, 10));
   io::println(~"pool size is " + int::to_str(pool_size, 10));
   io::println(~"web dir is " + web_dir);
}

