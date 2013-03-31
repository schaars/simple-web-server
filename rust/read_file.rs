/*
 * How to read a file.
 * Future work: as a variant, we may use the C bindings to call mmap/munmap
 */

/* read the file path by calling the read_whole_file_str function */
fn read_file_whole(path: ~str) -> ~str {
   let res = io::read_whole_file_str(&Path(path));
   if result::is_err(&res) {
      fail ~"file_reader error: " + result::get_err(&res);
   }
   res.get()
}

/* read the file path line by line */
fn read_file_lines(path: ~str) -> ~str {
   let res = io::file_reader(&Path(path));
   if result::is_err(&res) {
      fail ~"file_reader error: " + result::get_err(&res);
   }

   let mut content = ~"";
   let reader = res.get();
   loop {
      let line = (reader as io::ReaderUtil).read_line();
      if reader.eof() {
         break;
      }
      // read_line does not return the '\n', so we add it
      content += line + ~"\n";
   }

   content
}


fn main() {
   let filename = ~"read_file.rs";
   //let content = read_file_whole(copy filename);
   let content = read_file_lines(copy filename);
   io::println(~"the content of " + filename + ~" is [\n" + content + ~"]");
}
