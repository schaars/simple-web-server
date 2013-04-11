#include <stdio.h>
#include <stdlib.h>

// Should not be less than 256 to prevent a buffer overflow in scanf
#define MAXLINE 256
#define MAXBUF 8192

#define DEBUG 0

#define USE_SENDFILE 0

#if DEBUG
#define debug(msg, args...) \
   do {                         \
      printf("%c[%d;%dm(%s,%d) " msg "%c[%d;%dm\n", 0x1B, 0, 34, __FUNCTION__ , __LINE__, ##args, 0x1B, 0, 37); \
   } while(0)
#else
#define debug(msg, args...)
#endif

#define warn(msg, args...) \
   do {                         \
      fprintf(stderr, "%c[%d;%dm(%s,%d) " msg "%c[%d;%dm\n", 0x1B, 0, 33, __FUNCTION__ , __LINE__, ##args, 0x1B, 0, 37); \
      exit(-1);                 \
   } while(0)

#define die(msg, args...) \
   do {                         \
      fprintf(stderr, "%c[%d;%dm(%s,%d) " msg "%c[%d;%dm\n", 0x1B, 0, 31, __FUNCTION__ , __LINE__, ##args, 0x1B, 0, 37); \
      exit(-1);                 \
   } while(0)

