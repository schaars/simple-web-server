all: ws parse_arguments read_file echo_server

C:=rustc
CFLAGS=

ws: ws.rs
	$(C) $(CFLAGS) -o $@ $^

parse_arguments: parse_arguments.rs
	$(C) $(CFLAGS) -o $@ $^

read_file: read_file.rs
	$(C) $(CFLAGS) -o $@ $^

echo_server: echo_server.rs
	$(C) $(CFLAGS) -o $@ $^

clean:
	-rm *~
	-rm ws parse_arguments read_file echo_server
