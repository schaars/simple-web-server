#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "file_manager.h"
#include "pool.h"
#include "util.h"

static int listen_socket;

/*
 * Read a request from socket s in buffer req of size length.
 * Returns 0 if no error has occured, -1 if a syscall error
 * occurs, and 1 otherwise (if it's the fault of the client).
 */
int read_request(const int s, char *req, const int length) {
   int pos = 0;
   while (pos < length) {
      int r = read(s, req+pos, length-pos);
      req[length-1] = '\0';
      if (r == -1) {
         perror("Read error: ");
         return -1;
      } else if (r == 0) {
         return 1;      
      } else {
         if (strchr(req+pos, '\n')) {
            return 0;
         }
      }
      pos += r;
   }

   req[length-1] = '\0';
   return 1;
}

void write_complete(const int fd, const char *buf, const int length) {
   int written = 0;
   while (written < length) {
      int r = write(fd, buf+written, length-written);
      if (r == -1) {
         perror("Write error: ");
         return;
      } else {
         written += r;
      }
   }
}

void clienterror(const int fd, const char *cause, const char *errnum, 
      const char *shortmsg, const char *longmsg) 
{
   char buf[MAXLINE], body[MAXBUF];

   /* Build the HTTP response body */
   sprintf(body, "<html><title>Simple Web Server Error</title>");
   sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
   sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
   sprintf(body, "%s<p>%s: %s</p>\r\n", body, longmsg, cause);
   sprintf(body, "%s<hr><em>The Simple Web Server</em>\r\n", body);

   /* Print the HTTP response */
   sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
   write_complete(fd, buf, strlen(buf));
   sprintf(buf, "Content-type: text/html\r\n");
   write_complete(fd, buf, strlen(buf));
   sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
   write_complete(fd, buf, strlen(buf));
   write_complete(fd, body, strlen(body));
}

void clientsuccess(const int fd, const char *filename, const int length, const int status) {
   char filetype[MAXLINE], buf[MAXBUF];

   /* Send response headers to client */
   file_manager_get_filetype(filename, filetype);
   sprintf(buf, "HTTP/1.0 200 OK\r\n");
   sprintf(buf, "%sServer: Simple Web Server\r\n", buf);
   sprintf(buf, "%sContent-length: %d\r\n", buf, length);
   sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
   write_complete(fd, buf, strlen(buf));
}

void* start_thread(void *arg) {
   char request[MAXLINE];
   char op[MAXLINE];
   char file[MAXLINE];
   char version[MAXLINE];

   while (1) {
      struct sockaddr_in csin;
      int sinsize = sizeof(csin);
      int s = accept(listen_socket, (struct sockaddr*) &csin, (socklen_t*)&sinsize);
      if (s == -1) {
         perror("An invalid socket has been accepted:");
         continue;
      }

      debug("A connection has been accepted from %s:%i", inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));

      int r = read_request(s, request, MAXLINE);
      debug("request is %s", request);
      if (r == 0) {
         int m = sscanf(request, "%255s %255s %255s", op, file, version);
         op[MAXLINE-1] = file[MAXLINE-1] = version[MAXLINE-1] = '\0';

         if (m != 3) {
            clienterror(s, request, "400", "Bad request", "The request is not well-formatted");
         } else {
            if (strcmp(op, "GET")) {
               clienterror(s, request, "501", "Not implemented", "The request is not implemented");
            } else if (strcmp(version, "HTTP/1.0")) {
               clienterror(s, version, "505", "HTTP version not supported", "The server does not implement this version of HTTP");
            } else {
               int status, length;
#if USE_SENDFILE
               int fd = file_manager_get_sendfile(file, &status, &length);
               if (fd > 0) {
                  clientsuccess(s, file, length, status);
                  sendfile(s, fd, NULL, length);
                  file_manager_release_sendfile(fd);
#else
                  char *c = file_manager_get(file, &status, &length);
                  if (c != NULL) {
                     clientsuccess(s, file, length, status);
                     write_complete(s, c, length);
                     file_manager_release(c, length);
#endif
                  } else {
                     if (status == 404) {
                        clienterror(s, file, "404", "Not found", "The file cannot be found");
                     } else if (status == 401) {
                        clienterror(s, file, "401", "Not authorized", "The file cannot be accessed");
                     } else {
                        die("Internal error: unknown status %d", status);
                     }
                  }
               }
            }
         } else {
            clienterror(s, request, "400", "Bad request", "The request is not well-formatted");
         }

         close(s);
      }
   }

   /*
    * Create the pool of pool_size threads and launch the threads.
    * These threads will call the file_manager to deliver files
    * and listen to clients requests from the socket ls.
    */
   void create_pool(const int pool_size, int ls) {
      int i;

      listen_socket = ls;
      for (i=0; i<pool_size; i++) {
         pthread_t thread;
         int rc = pthread_create(&thread, NULL, start_thread, NULL);
         if (rc) {
            die("Failed to create the thread %d", i);
         }
      }
   }

   /*
    * Delete the pool of threads and free the associated memory
    */
   void delete_pool(void) {
   }
