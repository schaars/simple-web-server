simple-web-server
=================

A simple web-server, to play with the Rust programming language


C implementation
================

The C implementation is located in the directory c/

The source code is organized in the following files:

main.c: entry point. The piece of code which accepts new connections is located in this file.

shared_queue.[c+h]: a synchronized shared queue. It is used to send new connection file descriptors to the pool of threads.

pool.[c+h]: the pool of threads, which receive client requests and handle them. The threads use the file_manager in order to get the content of the requested files.

file_manager.[c+h]: several functions to open and manage the server files.

util.h: some constants and debugging stuff.

To compile the server:

 $ cd c/

 $ make


Rust implementation
===================

Work in progress...

The design should be similar, but in Rust style.


How to use the server
=====================

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

You can also use your favorite web browser, but make sure to send HTTP/1.0 requests, otherwise you will get a 505 HTTP version not supported error. For instance, in Firefox, you have 


Future work
===========

-We can improve the performance of the server by caching the files in memory.

