Simple Web Server
=================

A simple web-server, to play with the Rust programming language
The goal is not performance, but rather discovering the language and building a simple distributed system. There exists a lot of work on how to improve the performance of servers (e.g., [the C10k problem](http://en.wikipedia.org/wiki/C10k_problem)).  
The C server serves as a reference implementation: the Rust server must have the same functionalities.


Rust implementation
-------------------

Work in progress...  
The design should be similar to the C implementation, but in Rust style.


C implementation
----------------

The C implementation is located in the *c/* directory  
The source code is organized in the following files:  

* main.c: entry point. The piece of code which accepts new connections is located in this file.
* shared_queue.[c+h]: a synchronized shared queue. It is used to send new connection file descriptors to the pool of threads.
* pool.[c+h]: the pool of threads, which receive client requests and handle them. The threads use the file_manager in order to get the content of the requested files.
* file_manager.[c+h]: several functions to open and manage the server files.
* util.h: some constants and debugging stuff.


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

