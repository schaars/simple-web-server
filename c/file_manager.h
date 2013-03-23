#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_

/*
 * Create a new file_manager struct and return it.
 * web_dir represents the directory used as the root of the web server.
 */
void create_file_manager(char *web_dir);

/*
 * Delete the file_manager and free the associated memory
 */
void delete_file_manager(void);

/*
 * Retrieve and return the content of the file file, of size *length.
 * On success, *status=200. Later, one must call file_manager_release()
 * On failure, *status is set to an http code value and
 * NULL is returned.
 */
char* file_manager_get(const char *file, int *status, int *length);

/*
 * Free the memory allocated when calling file_manager_get
 * for buffer c of size length.
 */
void file_manager_release(char *c, const int length);

/*
 * get_filetype - derive file type from file name
 */
void file_manager_get_filetype(const char *filename, char *filetype);

#endif
