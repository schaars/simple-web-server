#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "file_manager.h"
#include "shared_queue.h"
#include "pool.h"
#include "util.h"

//We abruptly exit the program
//The kernel frees the resources for us :)
void handle_signal(int sig) {
   if (sig == SIGINT) {
      die("Exit on signal %d", sig);
   }
}

void accept_connections(const int port, struct shared_queue *sq) {
   int bootstrap = socket(AF_INET, SOCK_STREAM, 0);
   if (bootstrap == -1) {
      die("Error while creating the socket");
   }

   int flag = 1;
   if (setsockopt(bootstrap, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)) == -1) {
      perror("Cannot set TCP NO DELAY:");
   }

   struct sockaddr_in bootstrap_sin;
   bootstrap_sin.sin_addr.s_addr = htonl(INADDR_ANY);
   bootstrap_sin.sin_family = AF_INET;
   bootstrap_sin.sin_port = htons(port);
   if (bind(bootstrap, (struct sockaddr*)&bootstrap_sin, sizeof(bootstrap_sin)) == -1) {
      die("Cannot bind socket!");
   }

   if (listen(bootstrap, 50) == -1) {
      die("Cannot listen on socket!");
   }

   debug("Server ready to listen to incoming connections on port %d", port);

   while (1) {
      struct sockaddr_in csin;
      int sinsize = sizeof(csin);
      int s = accept(bootstrap, (struct sockaddr*) &csin, (socklen_t*)&sinsize);
      if (s == -1) {
         perror("An invalid socket has been accepted:");
         continue;
      }

      debug("A connection has been accepted from %s:%i", inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));

      shared_queue_add(s, sq);
   }
}

int main(int argc, char **argv) {
   int port = -1;
   int pool_size = 0;
   char* web_dir = NULL;

   int opt;
   while ((opt = getopt(argc, argv, "p:s:d:")) != EOF) {
      switch (opt) {
         case 'p':
            port = atoi(optarg);
            break;
         case 's':
            pool_size = atoi(optarg);
            break;
         case 'd':
            web_dir = strdup(optarg);
            break;
         default:
            die("Usage: %s -p port -s pool_size -d web_directory", argv[0]);
      }
   }

   if (port == -1 || pool_size == 0 || web_dir == NULL) {
      die("Usage: %s -p port -s pool_size -d web_directory", argv[0]);
   }

   signal(SIGINT, handle_signal);

   create_file_manager(web_dir);
   struct shared_queue *sq = create_shared_queue(10);
   create_pool(pool_size, sq);
   accept_connections(port, sq);

   delete_pool();
   delete_file_manager();
   delete_shared_queue(sq);
   free(web_dir);

   return 0;
}
