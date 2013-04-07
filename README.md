Simple Web Server
=================

A simple web-server, to play with the Rust programming language
The goal is not performance, but rather discovering the language and building a simple distributed system. There exists a lot of work on how to improve the performance of servers (e.g., [the C10k problem](http://en.wikipedia.org/wiki/C10k_problem)).  
The C server serves as a reference implementation: the Rust server must have the same functionalities.


Rust implementation
-------------------

The Rust implementation is located in the *rust/* directory. 
It has been successfully compiled and used with version 0.6 of the language:

    $ rustc --version
    rustc 0.6
    host: x86_64-unknown-linux-gnu

The source code is organized in the following files:

* parse_arguments.rs: a simple test on how to get and parse the command line arguments.
* read_file.rs: a simple test on how to read a file.
* echo_server.rs: a simple echo server in TCP.
* ws.rs: the simple web server

For now there is only 1 task to accept client connections. Moreover, the server does not make the difference between "file not found" and "not authorized": it returns a 404 error in both cases.


C implementation
----------------

The C implementation is located in the *c/* directory. 
The source code is organized in the following files:  

* main.c: entry point. The piece of code which accepts new connections is located in this file.
* shared_queue.[c+h]: a synchronized shared queue. It is used to send new connection file descriptors to the pool of threads.
* pool.[c+h]: the pool of threads, which receive client requests and handle them. The threads use the file_manager in order to get the content of the requested files.
* file_manager.[c+h]: several functions to open and manage the server files.
* util.h: some constants and debugging stuff.

Here are some keys to improve the performance of the C web server:

* use sendfile() instead of read() the file and then write() to socket;
* there is no need for a shared queue, nor an acceptor thread: each thread of the pool threads can call accept().


How to compile and use the server
---------------------------------

Both the C and Rust servers are compiled using the *make* command. The Makefile creates the *ws* executable in the source code folder.

To run the server:

    $ ./ws -p port -s pool_size -d web_dir

You can retrieve files using telnet:

    $ telnet server_ip server_port
    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    GET /test.txt HTTP/1.0
    HTTP/1.0 200 OK
    ... 

Or using curl:

    $ curl -0 http://server_ip:server_port/test.txt

You can also use your favorite web browser, but make sure to send HTTP/1.0 requests, otherwise you will get a "505 HTTP version not supported" error.
For instance, in Firefox, enter *about:config* in the address box, search for the entry *network.http.version*, modify its value to *1.0*, and restart Firefox.


The *www* directory contains several example files to test the web server.
