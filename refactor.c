#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define R_SUCCESS 0
#define R_INTERNAL_ERROR 1
#define R_ARGS_ERROR 2
#define R_SOURCE_NOT_FOUND 3
#define R_SOURCE_EMPTY 4
#define R_SOURCE_TYPE_ERROR 5
#define R_SUBSTR_NOT_FOUND 6

const char *help_msg = "\n NAME\n\trefactor - refactor strings in files and folders\n\n"
                       " SYNOPSIS\n\trefactor [OPTION]... [-i|-r|-d] [SOURCE] [FIND] [REPLACEMENT]\n\t"
                       "refactor [OPTION]... [-i]|[-r]|[-d] [SOURCE] [FIND] [REPLACEMENT]\n\t"
                       "refactor [OPTION]... [-h]\n\n"
                       " DESCRIPTION\n\tFind and replace all matching substrings in [SOURCE] where [FIND] is the"
                       " substring to search for\n\tand [REPLACEMENT] being the string to replace it with.\n\n"
                       " OPTIONS\n\tDefault usage without any options will result in a search of the substring"
                       "in the file provided as\n\t\tinput.\n\n\tEXAMPLE (1):\n\n\t\t$ refactor test.txt original changed"
                       "\n\n\t\tTranslates to : Find and replace all occurrences of the word 'original' in the"
                       " file 'test\n\t\t.txt' with the word 'changed'.\n\n\tEXAMPLE (2):\n\n\t\t$ refactor -ird"
                       " /home/$USER/Test/ ERROR SUCCESS\n\n\t\tTranslates to : Find and replace all occurrences of "
                       "the word 'ERROR' with the word 'SUCCE\n\t\tSS' in every file that '/home/$USER/Test/' contains."
                       "\n\n\t-h, --help\n\t\tShow help for the command"
                       "'s usage and exit.\n\n\t-i, --case-insensitive\n\t\tIgnore case of characters in the given"
                       " 'FIND' string when matching. By default, the comma\n\n\tnd will search for EXACT matches."
                       "\n\t-r, --recursive\n\t\tSearch recursively all subfolders and "
                       "replace all occurrences in files that match 'FIND'\n\t\twith 'REPLACEMENT'. Option '-d' MUST be declared as well!"
                       "\n\n\t-d, --directory\n\n\t\tParse all files in 'SOURCE' directory and replace all "
                       "occurrences that match 'FIND' with\n\t\t'REPLACEMENT'.\n\t\tIf the directory provided contains subdirectories"
                       ", '-d' will IGNORE them UNLESS '-r' is\n\t\tdeclared as well.\n\n RETURN VALUE\n\tUpon successful return, "
                       "the program will return 0.\n\tIf an error has occurred during the refactoring process, the program"
                       " will return a unique ERROR\n\tcode alongside an error message passed in stderr.\n\n\tSee the EXIT STATUS section"
                       " to view all possible ERROR codes.\n\n EXIT STATUS\n\t0:\tProgram executed successfully.\n\t"
                       "1:\tAn internal error has occurred with the c libraries used.\n\t2:\tInvalid usage of command"
                       ", command options are most likely not used properly.\n\t3:\tFile or folder not found.\n\t"
                       "4:\tFile or folder empty.\n\t5:\tSource is neither a file or folder.\n\t6:\tSubstring"
                       " [FIND] did not match anything.\n\n CONFORMING TO\n\tC99, C11, C17, POSIX.1-2001, POSIX.1-2008\n\n"
                       " AUTHOR\n\tWritten by Nami Reghbati.\n\n COPYRIGHT\n\tCopyright © 2020 Free Software Foundation,"
                       " Inc. License  GPLv3+:  GNU  GPL  version  3  or  later\n\t<https://gnu.org/licenses/gpl.html>.\n"
                       "\tThis is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the"
                       "\n\textent permitted by law.\n\n";

typedef struct args_s
{
  bool i_option;  // Case-insensitive.
  bool d_option;  // Refactor all specified directory files.
  bool r_option;  // Follow subdirectories recursively.
  const char *filename;
  char *needle;
  const char *replacement;
} args_t;

typedef struct file_s
{
  unsigned int status_code;
  char *error_msg;
} file_t;

args_t read_cmd(int argc, char **argv);
file_t check_input(args_t args);
file_t find_in_file(args_t args);
void to_uppercase(char *buffer);
file_t replace_in_dir(args_t args);
file_t replace_in_file(args_t args, char *buffer_file);

args_t read_cmd(int argc, char **argv)
{
  if (argc < 2 || argc > 8)
  {
    fprintf(stderr, "ERROR: INVALID COMMAND ARGUMENTS PROVIDED!\t"
                    " RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\tEXITING...\n");
    exit(R_ARGS_ERROR);
  }
  args_t opt = {false, false, false, NULL, NULL, NULL};
  for (int i = 1; i < argc; i++)
  {
    if (argv[i] && argv[i][0] == '-')
    {
      for (int j = 0; j < strlen(argv[i]); j++)
      {
        if (argv[i][1] == '-')
        {
          if (strcmp(argv[i], "--help") == 0)
          {
            fprintf(stdout, "USAGE : [OPTIONS : -h|-r|-d] [FILE] [FIND_STR] [REPLACEMENT_STR]\n");
            exit(R_SUCCESS);
          }
          else if (strcmp(argv[i], "--directory") == 0)
          {
            opt.d_option = true;
            break;
          }
          else if (strcmp(argv[i], "--recursive") == 0)
          {
            opt.r_option = true;
            break;
          }
        }
        switch (argv[i][1])
        {
          case 'i':
          {
            opt.i_option = true;
            break;
          }
          case 'd':
          {
            opt.d_option = true;
            break;
          }
          case 'r':
          {
            opt.r_option = true;
            break;
          }
          case 'h':
          {
            fprintf(stdout, "%s", help_msg);
            exit(R_SUCCESS);
          }
          default:
          {
            fprintf(stderr, "ERROR: INVALID COMMAND ARGUMENTS PROVIDED!\t"
                            " RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\tEXITING...\n");
            exit(R_ARGS_ERROR);
          }
        }
      }
    }
    else if (!opt.filename) opt.filename = argv[i];
    else if (!opt.needle) opt.needle = argv[i];
    else if (!opt.replacement) opt.replacement = argv[i];
  }
  if (opt.r_option && !opt.d_option)  // Recursive without directory search.
  {
    fprintf(stderr, "ERROR: INVALID COMMAND ARGUMENTS PROVIDED!\n\tOPTION 'r' PROVIDED WITHOUT 'd'!\n"
                    "RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\tEXITING...\n");
    exit(R_ARGS_ERROR);
  }
  return opt;
}

file_t check_input(args_t args)
{
  struct stat stat_p;
  stat(args.filename, &stat_p);
  if (!S_ISREG(stat_p.st_mode) && !S_ISDIR(stat_p.st_mode))
    return (file_t) {R_SOURCE_TYPE_ERROR, "ERROR: INVALID FILE TYPE!\t"
                                          "EXITING...\n"};
  FILE *file = fopen(args.filename, "r");
  if (!file)
    return (file_t) {R_SOURCE_NOT_FOUND, "ERROR: COULD NOT OPEN FILE!"
                                         " CHECK SPELLING AND FILE RIGHTS.\tEXITING...\n"};
  fclose(file);
  return (file_t) {R_SUCCESS, NULL};
}

file_t find_in_file(args_t args)
{
  if (!strcmp(args.needle, args.replacement)) return (file_t) {R_SUCCESS, NULL};
  FILE *file = fopen(args.filename, "r");
  if (!file)
    return (file_t) {R_SOURCE_NOT_FOUND, "ERROR: COULD NOT OPEN FILE! CHECK SPELLING AND FILE RIGHTS...\n"};
  char *buffer = (char *) calloc(1, FILENAME_MAX * 4000);  // 16 MBs.
  fread(buffer, FILENAME_MAX * 4000, 1, file);

  if (ferror(file))
  {
    fclose(file);
    free(buffer);
    return (file_t) {R_SOURCE_EMPTY, "ERROR: FILE EMPTY!\n"};
  }
  if (!strstr(buffer, args.needle))
  {
    fclose(file);
    free(buffer);
    return (file_t) {R_SUBSTR_NOT_FOUND, "ERROR: SUBSTRING NOT FOUND IN FILE!\n"};
  }
  file_t result = replace_in_file(args, buffer);
  if (result.status_code)
  {
    fclose(file);
    free(buffer);
    return result;
  }
  file = fopen(args.filename, "w");  // Reopen file and erase its contents.
  if (!file)
  {
    fclose(file);
    free(buffer);
    return (file_t) {R_SOURCE_NOT_FOUND, "ERROR: COULD NOT OPEN FILE! CHECK SPELLING AND FILE RIGHTS...\n"};
  }
  fwrite(buffer, strlen(buffer), 1, file);
  fclose(file);
  free(buffer);
  return (file_t) {R_SUCCESS, NULL};
}

file_t replace_in_file(args_t args, char *buffer_file)
{
  long index;
  char *cursor_pos;
  char *temp = (char *) calloc(1, FILENAME_MAX * 4000);
  if (!strncpy(temp, buffer_file, strlen(buffer_file)))
    return (file_t) {R_INTERNAL_ERROR,
                     "ERROR: [replace_in_file][strncpy] : Could not"
                     "copy file contents!\n"};
  if (args.i_option)  // Capitalize both needle and buffer to force matching.
  {
    to_uppercase(args.needle);
    to_uppercase(temp);
  }
  while ((cursor_pos = strstr(temp, args.needle)))  // Parse the buffer and check for matches.
  {
    if (!strncpy(temp, buffer_file, strlen(buffer_file)))
      return (file_t) {R_INTERNAL_ERROR, "ERROR: [replace_in_file][strncpy] : Could not copy file contents!\n"};
    index = cursor_pos - temp;  // Index of current found word
    buffer_file[index] = '\0';  // Terminate str after word found index.
    if (!strncat(buffer_file, args.replacement, strlen(buffer_file) + strlen(args.replacement)))
      return (file_t) {R_INTERNAL_ERROR,
                       "ERROR: [replace_in_file][strncat] : Could not concatenate file contents!\n"};  // Concatenate str with new word.
    if (!strncat(buffer_file, temp + index + strlen(args.needle),
                 strlen(buffer_file) + strlen(temp)))
      return (file_t) {R_INTERNAL_ERROR,
                       "ERROR: [replace_in_file][strncat] : Could not concatenate file contents!\n"};  // Concatenate str with remaining words after.
    if (args.i_option)
    {
      if (!strncpy(temp, buffer_file, strlen(buffer_file)))
        return (file_t) {R_INTERNAL_ERROR,
                         "ERROR: [replace_in_file][strncpy] : Could not copy file contents!\n"};  // Update buffer length for next cursor pos.
      to_uppercase(temp + index + strlen(args.replacement));  // Uppercase remaining buffer str.
    }
  }
  free(temp);
  return (file_t) {R_SUCCESS, NULL};
}

// Recursive function to check each directory file containing substring to refactor.
file_t replace_in_dir(args_t args)
{
  if (!args.d_option) return (file_t) {R_ARGS_ERROR, "ERROR: DIRECTORY GIVEN AS FILE WITHOUT"
                                                     " THE -d | --directory ARGUMENT!\tEXITING...\n"};
  struct dirent *d;  // Directory entries.
  DIR *dir = opendir(args.filename);
  if (!dir)
    return (file_t) {R_SOURCE_NOT_FOUND, "ERROR: UNABLE TO READ DIRECTORY!"
                                         " CHECK SPELLING AND FILE RIGHTS...\n"};
  file_t result;
  while ((d = readdir(dir)))
  {
    result = (file_t) {R_SUCCESS, NULL};
    char buffer[strlen(args.filename) + strlen(d->d_name) + 2];
    snprintf(buffer, strlen(args.filename) + strlen(d->d_name) + 2, "%s/%s", args.filename, d->d_name);
    struct stat stat_p;
    stat(buffer, &stat_p);
    if (S_ISDIR(stat_p.st_mode) && args.r_option && strcmp(d->d_name, "..") != 0 &&
        strcmp(d->d_name, ".") != 0)
      return replace_in_dir(args);
    if (!S_ISDIR(stat_p.st_mode) && strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0)
      result = find_in_file(
          (args_t) {args.i_option, args.d_option, args.r_option, buffer, args.needle, args.replacement});
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

void to_uppercase(char *buffer)
{
  // Convert to upper case
  for (int i = 0; i < strlen(buffer); ++i) buffer[i] = (char) toupper((unsigned char) buffer[i]);
}

int main(int argc, char **argv)
{
  args_t cmd_args = read_cmd(argc, argv);
  file_t result = check_input(cmd_args);
  if (result.status_code != R_SUCCESS)
  {
    fprintf(stderr, "%s", result.error_msg);
    return (int) result.status_code;
  }
  if (cmd_args.d_option) result = replace_in_dir(cmd_args);
  if (!cmd_args.d_option) result = find_in_file(cmd_args);
  if (result.status_code) fprintf(stderr, "%s", result.error_msg);
  return (int) result.status_code;
}