#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>

#define R_SUCCESS 0
#define R_ARGS_ERROR -1
#define R_FILE_NOT_FOUND -2
#define R_FILE_EMPTY -3
#define R_STR_NOT_FOUND -4
#define R_ERROR_DIR -5
#define R_ERROR_FILE_TYPE -6

// File types parsed during refactoring process.
//#define T_FILE 2
//#define T_FOLDER 3
//#define T_UNKNOWN 4

typedef struct args_s
{
  bool d_option;
  bool r_option;
  const char *filename;
  const char *needle;
  const char *replacement;
} args_t;

typedef struct file_s
{
  int status_code;
  char *error_msg;
} file_t;

args_t read_cmd(int argc, char **argv);
file_t check_input(args_t args);
file_t refactor_dir(args_t args);
file_t refactor_file(args_t args);

args_t read_cmd(int argc, char **argv)
{
  if (argc > 7)
  {
    fprintf(stderr, "ERROR: INVALID COMMAND ARGUMENTS PROVIDED!\t"
                    " RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\tEXITING...\n");
    exit(R_ARGS_ERROR);
  }
  args_t opt = {false, false, NULL, NULL, NULL};
  for (int i = 1; i < argc; i++)
  {
    if (argv[i] && argv[i][0] == '-')
    {
      for (int j = 0; j < strlen(argv[i]); j++)
      {
        if (argv[i][1] == '-')
        {
          if (strcmp(argv[i], "--help") == 0) {
            fprintf(stdout, "USAGE : [OPTIONS : -h|-r|-d] [FILE] [FIND_STR] [REPLACEMENT_STR]\n");
            exit(R_SUCCESS);
          }
          else if (strcmp(argv[i], "--directory") == 0) { opt.d_option = true; break; }
          else if (strcmp(argv[i], "--recursive") == 0) { opt.r_option = true; break; }
        }
        switch (argv[i][1])
        {
          case 'd': { opt.d_option = true; break; }
          case 'r': { opt.r_option = true; break; }
          case 'h': {
            fprintf(stdout, "USAGE : [OPTIONS : -h|-r|-d] [FILE] [FIND_STR] [REPLACEMENT_STR]\n");
            exit(R_SUCCESS);
          }
          default: {
              fprintf(stderr, "ERROR: INVALID COMMAND ARGUMENTS PROVIDED!\t"
                              " RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\tEXITING...\n");
            exit(R_ARGS_ERROR);
          }
        }
      }
    } else if (!opt.filename) opt.filename = argv[i];
    else if (!opt.needle) opt.needle = argv[i];
    else if (!opt.replacement) opt.replacement = argv[i];
  }
  return opt;
}

file_t check_input(args_t args)
{
  struct stat stat_p;
  stat(args.filename, &stat_p);
  if (!S_ISREG(stat_p.st_mode) && !S_ISDIR(stat_p.st_mode)) return (file_t) {R_ERROR_FILE_TYPE,
                                                                             "ERROR: INVALID FILE TYPE!\t"
                                                                             "EXITING...\n"};
  FILE *file = fopen(args.filename, "r");
  if (!file)
    return (file_t) {R_FILE_NOT_FOUND, "ERROR: COULD NOT OPEN FILE!"
                                       " CHECK SPELLING AND FILE RIGHTS.\tEXITING...\n"};
  fclose(file);
  return (file_t) {R_SUCCESS, "FILE FOUND!\tPROCEEDING TO REFACTOR...\n"};
}

// Recursive function to check each directory file containing substring to refactor.
file_t refactor_dir(args_t args)
{
  if (!args.d_option) return (file_t) {R_ARGS_ERROR, "ERROR: DIRECTORY GIVEN AS FILE WITHOUT"
                                                              " THE -d | --directory ARGUMENT!\tEXITING...\n"};
  struct dirent *d;  // Directory entries.
  DIR *dir = opendir(args.filename);
  if (!dir) return (file_t) {R_ERROR_DIR, "ERROR: UNABLE TO READ DIRECTORY!"
                                          " CHECK SPELLING AND FILE RIGHTS...\n"};
  file_t result;
  while ((d = readdir(dir)))
  {
    result = (file_t) {R_SUCCESS, NULL};
    char buffer[256];
    snprintf(buffer, 256, "%s/%s", args.filename, d->d_name);
    struct stat stat_p;
    stat(buffer, &stat_p);
    if (S_ISDIR(stat_p.st_mode) && args.r_option) return refactor_dir(args);
    if(!S_ISDIR(stat_p.st_mode) && strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0)
      result = refactor_file((args_t) {args.d_option, args.r_option, buffer, args.needle, args.replacement});
      // If error occurred.
    if (errno || result.status_code)
    {
      if (errno) perror("ERROR");
      fprintf(stderr, "ERROR: [%s] %s", buffer, result.error_msg);
    }
  }
  closedir(dir);
  return result;
}

file_t refactor_file(args_t args)
{
  if (!strcmp(args.needle, args.replacement)) return (file_t) {R_SUCCESS, NULL};
  FILE *file = fopen(args.filename, "r");
  if (!file)
    return (file_t) {R_FILE_NOT_FOUND, "ERROR: COULD NOT OPEN FILE!"
                                             " CHECK SPELLING AND FILE RIGHTS...\n"};
  char buffer[FILENAME_MAX * 4];  // 16 MBs.
  fread(buffer, FILENAME_MAX * 4, 1, file);

  if (ferror(file)) return (file_t) {R_FILE_EMPTY, "ERROR: FILE EMPTY!\n"};
  if (!strstr(buffer, args.needle)) return (file_t) {R_STR_NOT_FOUND,
                                                     "ERROR: SUBSTRING NOT FOUND IN FILE!\n"};

  long index;
  char *pos, temp[FILENAME_MAX * 4];
  /*
   * Repeat till all occurrences are replaced.
   */
  while ((pos = strstr(buffer, args.needle)) != NULL)
  {
    // Backup current line
    strcpy(temp, buffer);

    // Index of current found word
    index = pos - buffer;

    // Terminate str after word found index
    buffer[index] = '\0';

    // Concatenate str with new word
    strcat(buffer, args.replacement);

    // Concatenate str with remaining words after
    strcat(buffer, temp + index + strlen(args.needle));
  }
  file = fopen(args.filename, "w");  // Reopen file and erase its contents.
  if (!file) return (file_t) {R_FILE_NOT_FOUND, "ERROR: COULD NOT OPEN FILE!"
                                                       " CHECK SPELLING AND FILE RIGHTS...\n"};
  fwrite(buffer, strlen(buffer), 1, file);
  fclose(file);
  return (file_t) {R_SUCCESS, NULL};
}

int main(int argc, char **argv)
{
  args_t cmd_args = read_cmd(argc, argv);
  file_t result = check_input(cmd_args);
  if (result.status_code != R_SUCCESS)
  {
    fprintf(stderr, "%s", result.error_msg);
    return result.status_code;
  }
  if (cmd_args.d_option) result = refactor_dir(cmd_args);
  if (!cmd_args.d_option && (result = refactor_file(cmd_args)).status_code)
    fprintf(stderr, "%s", result.error_msg);
  return result.status_code;
}