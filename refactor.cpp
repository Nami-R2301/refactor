#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cctype>
#include "logger.h"

#define R_SUCCESS 0
#define R_INTERNAL_ERROR 1
#define R_ARGS_ERROR 2
#define R_SOURCE_NOT_FOUND 3
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

typedef struct refactor_s
{
  bool i_option;  // Case-insensitive.
  bool d_option;  // Refactor all specified directory files.
  bool r_option;  // Follow subdirectories recursively.
  char *filename;
  char *needle;
  const char *replacement;
} refactor_t;

refactor_t read_cmd(int argc, char **argv);
int check_input(refactor_t args);
void to_uppercase(char *buffer);
void replace_in_dir(refactor_t args);
void replace_in_file(refactor_t args);
void error_handling(const char *msg, int code);

refactor_t read_cmd(int argc, char **argv)
{
  if (argc < 2 || argc > 8)
    error_handling("INVALID COMMAND ARGUMENTS PROVIDED!\t RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\t"
                   "EXITING...\n", R_ARGS_ERROR);

  refactor_t opt = {false, false, false, nullptr, nullptr, nullptr};
  for (int i = 1; i < argc; i++)
  {
    if (argv[i] && argv[i][0] == '-')
    {
      for (int j = 1; j < (int) strlen(argv[i]); j++)
      {
        if (argv[i][1] == '-')  // --OPTION
        {
          if (strcmp(argv[i], "--help") == 0)
          {
            Nami::alert(LOG_WARN, "USAGE : [OPTIONS : -h|-r|-d] [FILE] [FIND_STR] [REPLACEMENT_STR]\n");
            exit(R_SUCCESS);
          } else if (strcmp(argv[i], "--directory") == 0)
          {
            opt.d_option = true;
            break;
          } else if (strcmp(argv[i], "--recursive") == 0)
          {
            opt.r_option = true;
            break;
          } else if (strcmp(argv[i], "--log") == 0)
          {
            Nami::print_logs();
            exit(R_SUCCESS);
          }
        }
        switch (argv[i][j])  // -i|d|r|h
        {
          case 'l':
            Nami::print_logs();
            exit(R_SUCCESS);
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
            printf("%s", help_msg);
            exit(R_SUCCESS);
          }
          default:
            error_handling(
                "INVALID COMMAND ARGUMENTS PROVIDED!\t RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\t"
                "EXITING...\n", R_ARGS_ERROR);
        }
      }
    } else if (!opt.filename) opt.filename = argv[i];
    else if (!opt.needle) opt.needle = argv[i];
    else if (!opt.replacement) opt.replacement = argv[i];
  }
  if (opt.r_option && !opt.d_option)  // Recursive without directory search.
  {
    Nami::alert(LOG_ERROR, "INVALID COMMAND ARGUMENTS PROVIDED!\n\tOPTION 'r' PROVIDED WITHOUT 'd'!\n"
                           "RE_RUN USING -h OR --help TO VIEW POSSIBLE ARGUMENTS.\tEXITING...\n");
    exit(R_ARGS_ERROR);
  }
  return opt;
}

int check_input(refactor_t args)
{
  struct stat stat_p{};
  stat(args.filename, &stat_p);
  if (!S_ISREG(stat_p.st_mode) && !S_ISDIR(stat_p.st_mode))
    error_handling("Invalid file type!\t Exiting ...", R_SOURCE_TYPE_ERROR);
  if (!strcmp(args.needle, args.replacement)) return R_SUCCESS;
  if (!S_ISDIR(stat_p.st_mode))
  {
    FILE *file = fopen(args.filename, "r");
    if (!file)
      error_handling("Could not open file! Check file path spelling and rights ...", R_SOURCE_TYPE_ERROR);
    char *buffer = (char *) calloc(1, FILENAME_MAX * 4000);  // 16 MBs.
    fread(buffer, FILENAME_MAX * 4000, 1, file);

    if (ferror(file) || !strstr(buffer, args.needle))
    {
      fclose(file);
      free(buffer);
      Nami::alert(LOG_WARN, "Substring not found in file!");
      return R_SUBSTR_NOT_FOUND;
    }
    fclose(file);
    free(buffer);
  }
  return R_SUCCESS;
}

void replace_in_file(refactor_t args)
{
  unsigned long index = 0;
  char *cursor_pos;
  FILE *file = fopen(args.filename, "r+");
  if (!file)
    return error_handling("Could not open file! Check file path spelling and rights ...", R_SOURCE_TYPE_ERROR);
  char *temp = (char *) calloc(1, FILENAME_MAX * 4000);
  char *buffer = (char *) calloc(1, FILENAME_MAX * 4000);  // 16 MBs.
  fread(buffer, FILENAME_MAX * 4000, 1, file);

  if (!strncpy(temp, buffer, strlen(buffer)))
  {
    free(temp);
    free(buffer);
    fclose(file);
    return error_handling("[replace_in_file][strncpy] : Could not copy file contents", R_INTERNAL_ERROR);
  }
  if (args.i_option)  // Capitalize both needle and buffer to force matching.
  {
    to_uppercase(args.needle);
    to_uppercase(temp);
  }
  while ((cursor_pos = strstr(temp + index, args.needle)))  // Parse the buffer and check for matches.
  {
    index = cursor_pos - temp;  // Index of current found word
    buffer[index] = '\0';  // Terminate str after word found index.

    // Concatenate str with new word.
    if (!strncat(buffer, args.replacement, strlen(buffer) + strlen(args.replacement)))
    {
      free(buffer);
      fclose(file);
      return error_handling("[strncat] : Could not copy file contents", R_INTERNAL_ERROR);
    }
    // Concatenate str with remaining words after.
    if (!strncat(buffer, temp + index + strlen(args.needle),
                 strlen(buffer) + strlen(temp)))
    {
      free(buffer);
      fclose(file);
      return error_handling("[strncat] : Could not copy file contents", R_INTERNAL_ERROR);
    }
    if (args.i_option)
    {
      // Update buffer length for next cursor pos.
      if (!strncpy(temp, buffer, strlen(buffer)))
      {
        free(buffer);
        fclose(file);
        return error_handling("[strncpy] : Could not copy file contents", R_INTERNAL_ERROR);
      }
      to_uppercase(temp + index + strlen(args.replacement));  // Uppercase remaining buffer str.
    }
    if (!strncpy(temp, buffer, strlen(buffer)))
    {
      free(buffer);
      return error_handling("[strncpy] : Could not copy file contents", R_INTERNAL_ERROR);
    }
    index += strlen(args.replacement);
  }
  free(temp);
  fwrite(buffer, strlen(buffer), 1, file);
  free(buffer);
  fclose(file);
}

// Recursive function to check each directory file containing substring to refactor.
void replace_in_dir(refactor_t args)
{
  if (!args.d_option)
    return error_handling("Directory passed in without -d | --directory option", R_ARGS_ERROR);
  struct dirent *d;  // Directory entries.

  DIR *dir = opendir(args.filename);
  if (!dir)
    return error_handling("Unable to read directory %s! Check directory path and directory rights", R_SOURCE_NOT_FOUND);

  while ((d = readdir(dir)))
  {
    char *buffer = (char *) calloc(1, FILENAME_MAX);
    if (!buffer)
    {
      free(buffer);
      return error_handling("[Calloc] : Could not allocate dynamic memory", ERROR_HEAP_ALLOC);
    }
    // If filename contains a forward slash, remove it for recursive concatenation of next children down the path.
    if (args.filename[strlen(args.filename) - 1] == '/') args.filename[strlen(args.filename) - 1] = '\0';
    if (snprintf(buffer, FILENAME_MAX, "%s/%s", args.filename, d->d_name) < 0)
    {
      free(buffer);
      return error_handling("[snprintf] : Could not concatenate strings", ERROR_SNPRINTF);
    }

    Nami::alert(LOG_INFO, "Refactoring %s ...", buffer);
    struct stat stat_p{};
    stat(buffer, &stat_p);
    refactor_t current_inode = {args.i_option, args.d_option, args.r_option, buffer,
                                args.needle, args.replacement};
    if (S_ISDIR(stat_p.st_mode) && args.r_option && strcmp(d->d_name, "..") != 0 &&
        strcmp(d->d_name, ".") != 0)
      replace_in_dir(current_inode);
    else if (strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0) replace_in_file(current_inode);
    if (strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0)
      Nami::alert(LOG_STATUS_DONE, "%s refactored successfully", buffer);
    free(buffer);
  }
  closedir(dir);
}

void to_uppercase(char *buffer)
{
  // Convert to upper case
  for (int i = 0; i < (int) strlen(buffer); ++i) buffer[i] = (char) toupper((unsigned char) buffer[i]);
}

void error_handling(const char *msg, int code)
{
  Nami::alert(LOG_ERROR, "%s at line %d!", msg, __LINE__);
  exit(code);
}

int main(int argc, char **argv)
{
  Nami::open_file();
  refactor_t refactor_options = read_cmd(argc, argv);
  int result = check_input(refactor_options);
  if (!result && !refactor_options.d_option) replace_in_file(refactor_options);
  else if (!result) replace_in_dir(refactor_options);
  Nami::close_file();
  return R_SUCCESS;
}