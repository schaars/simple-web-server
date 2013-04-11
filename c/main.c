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
#include "pool.h"
#include "util.h"

//We abruptly exit the program
//The kernel frees the resources for us :)
void handle_signal(int sig) {
   if (sig == SIGINT) {
      die("Exit on signal %d", sig);
   }
}

int accept_connections(const int port) {
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

   if (listen(bootstrap, 100) == -1) {
      die("Cannot listen on socket!");
   }

   debug("Server ready to listen to incoming connections on port %d", port);
   return bootstrap;
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
   int bootstrap_s = accept_connections(port);
   create_pool(pool_size, bootstrap_s);

   while (1) {
      sleep(10);
   }

   delete_pool();
   delete_file_manager();
   free(web_dir);

   return 0;
}
