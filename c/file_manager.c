#define _GNU_SOURCE

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "file_manager.h"
#include "util.h"

static char *webdir;

/*
 * Create a new file manager.
 * web_dir represents the directory used as the root of the web server.
 * Its content is never copied: the caller must not free this variable!
 */
void create_file_manager(char *web_dir) {
   webdir = web_dir; 
}

/*
 * Delete the file manager and free the associated memory
 */
void delete_file_manager() {
}

/*
 * Retrieve and return the content of the file file, of size *length.
 * On success, *status=200. Later, one must call file_manager_release()
 * On failure, *status is set to an http code value and
 * NULL is returned.
 */
char* file_manager_get(const char *file, int *status, int *length) {
   char *path;
   asprintf(&path, "%s%s", webdir, file);

   struct stat st;
   if (stat(path, &st) < 0) {
      *status = 404;
      free(path);
      return NULL;
   }
   *length = st.st_size;

   int fd;
   if ((fd = open(path, O_RDONLY)) == -1)
   {
      *status = 401;
      free(path);
      return NULL;
   }

   char *c = (char*) mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
   if (c == MAP_FAILED)
   {

      die("mmap %s size %ld fail: %s\n", path, (long) st.st_size, strerror(errno));
   }

   *status = 200;

   close(fd);
   free(path);
   return c;
}

/*
 * Free the memory allocated when calling file_manager_get
 * for buffer c of size length.
 */
void file_manager_release(char *c, const int length) {
   munmap(c, length);
}


/*
 * get_filetype - derive file type from file name
 */
void file_manager_get_filetype(const char *filename, char *filetype) 
{
   if (strstr(filename, ".html")) {
      strcpy(filetype, "text/html");
   } else if (strstr(filename, ".gif")) {
      strcpy(filetype, "image/gif");
   } else if (strstr(filename, ".jpg")) {
      strcpy(filetype, "image/jpeg");
   } else {
      strcpy(filetype, "text/plain");
   }
} 

