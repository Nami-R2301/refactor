#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#define R_VALID_CMD 1
#define R_INVALID_CMD 2

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
  bool h_option;
  const char *filename;
  const char *needle;
} args_t;

typedef struct find_s
{
  int status_code;
  char *error_msg;
} find_t;

args_t read_cmd(int argc, char **argv);
find_t find_input(args_t args);
int read_dir(char *dirname);
int refactor(char *filename);

args_t read_cmd(int argc, char **argv)
{
  args_t opt = {false, false, false, NULL, NULL};
  for (int i = 1; i < argc; i++)
  {
    if (argv[i] && argv[i][0] == '-')
    {
      if (strcmp(argv[i], "--help") == 0) { opt.h_option = true; break; }
      else if (strcmp(argv[i], "--directory") == 0) { opt.d_option = true; break; }
      else if (strcmp(argv[i], "--recursive") == 0) { opt.r_option = true; break; }
      for (int j = 0; j < strlen(argv[i]); j++)
      {
        switch (argv[i][1])
        {
          case 'd': { opt.d_option = true; break; }
          case 'r': { opt.r_option = true; break; }
          case 'h': { opt.h_option = true; break; }
          default: break;
        }
      }
    } else if (!opt.filename) opt.filename = argv[i];
    else if (!opt.needle) opt.needle = argv[i];
  }
  return opt;
}

find_t find_input(args_t args)
{
  FILE *file = fopen(args.filename, "r");
  if (!file)
    return (find_t) {R_FILE_NOT_FOUND, "ERROR: FILE NOT FOUND\t CHECK SPELLING AND TRY AGAIN...\n"};
  char buffer[FILENAME_MAX * 4];  // 16 MBs.
  fread(buffer, FILENAME_MAX * 4, 1, file);

  if (ferror(file)) return (find_t) {R_FILE_EMPTY, "ERROR: FILE EMPTY!\n"};
  if (!strstr(buffer, args.needle)) return (find_t) {R_STR_NOT_FOUND, "ERROR: SUBSTRING NOT FOUND IN FILE!\n"};

  return (find_t) {R_SUCCESS, "FILE FOUND!\tPROCEEDING TO REFACTOR...\n"};
}

int read_dir(char *dirname)
{
  struct dirent *d;  // Directory entries.
  DIR *dir = opendir(dirname);
  if (!dir)
  {
    // If the directory is not found
    if (errno == ENOENT) perror("Directory doesn't exist");
      // If the directory is not readable then throw error and exit
    else perror("Unable to read directory");
    return R_ERROR_DIR;
  }
  while ((d = readdir(dir))) refactor(d->d_name);
  return 0;
}

int refactor(char *filename)
{
  return 0;
}

int main(int argc, char **argv)
{
  if (argc < 4)
  {
    fprintf(stderr, "ERROR: INVALID COMMAND ARGUMENTS PROVIDED!\t"
                    " re-run using -h or --help to view possible arguments\n");
    return R_ARGS_ERROR;
  }
  args_t cmd_args = read_cmd(argc, argv);
  find_t result = find_input(cmd_args);
  if (result.status_code) fprintf(stderr, "%s", result.error_msg);

  struct stat stat_p;
  stat(cmd_args.filename, &stat_p);
  if (S_ISDIR(stat_p.st_mode)) return read_dir(argv[1]);
  if (!S_ISREG(stat_p.st_mode)) return R_ERROR_FILE_TYPE;
  result.status_code = refactor(argv[1]);
  return result.status_code;
}