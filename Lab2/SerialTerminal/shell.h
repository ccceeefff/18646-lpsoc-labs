#include <Arduino.h>

/*
 * Help Functions
 */

/* Prints out the list of available commands */
void sh_help(Stream *stream, const char *params[], int paramCount);


/*
 * Directory Functions
 */

/* Prints out the current working directory */
void sh_pwd(Stream *stream, const char *params[], int paramCount);

/* Changes the current working directory */
void sh_cd(Stream *stream, const char *params[], int paramCount);

/* List all files in current working directory or target directory */
void sh_ls(Stream *stream, const char *params[], int paramCount);

/* Creates a new directory if it does not exist */
void sh_mkdir(Stream *stream, const char *params[], int paramCount);

/*
 * File Functions
 */

/* Prints out the contents of a file*/
void sh_cat(Stream *stream, const char *params[], int paramCount);

/* Deletes a specified file */
void sh_rm(Stream *stream, const char *params[], int paramCount);

/* Moves or renames a file */
void sh_mv(Stream *stream, const char *params[], int paramCount);


/* Current working directory holder */
extern String PWD;
