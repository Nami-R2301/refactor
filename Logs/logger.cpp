//
// Created by nami on 5/16/22.
//

#include "logger.h"
#include <fstream>

namespace Nami
{

  static FILE *file_ptr = nullptr;

  void open_file()
  {
    file_ptr = fopen("log.txt", "a");
    if (!file_ptr)
    {
      alert(LOG_WARN, "ERROR OPENING FILE STREAM! INVALID PATH!");
    }
  }

  void print_logs()
  {
    std::ifstream in(("log.txt"), std::ios::binary);
    if (in)
    {
      std::string contents;
      in.seekg(0, std::ios::end);
      contents.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], (long) contents.size());
      in.close();
      fflush(stdout);
      printf("%s\n------------START OF LOGS--------------\n%s%s",
             GREEN, contents.c_str(), RESET);
      printf("%s------------END OF LOGS--------------%s\n", GREEN, RESET);
    } else alert(LOG_WARN, "NO LOGS DOCUMENTED YET!\tTRY LAUNCHING THE PROGRAM AT LEAST ONCE...");
  }

  void reset_logs()
  {
    open_file();
    int result = 0;
    if (file_ptr)
      result = remove("log.txt");
    fflush(stdout);
    if (result == 0) alert(LOG_STATUS_DONE, "LOG FILE RESET!\tPLEASE RUN THE PROGRAM AGAIN TO VIEW LOGS...");
    else
      alert(LOG_ERROR, "%s[LOG ERROR] : CAN'T RESET LOG FILE!\tMAKE SURE YOU RAN THE PROGRAM AT "
                       "LEAST ONCE...");
  }

  void save_to_file(const char *info)
  {
    if (file_ptr)
    {
      fprintf(file_ptr, "%s\n", info);
    }
  }

  void alert(int info_type, const char *format, ...)
  {
    va_list args;
    va_start(args, format);

    char *string;
    vasprintf(&string, format, args);

    char type[35];
    if (info_type == LOG_INFO) snprintf(type, 15, "%s[INFO]", WHITE);
    else if (info_type == LOG_ERROR) snprintf(type, 16, "%s[ERROR]", RED);
    else if (info_type == LOG_WARN) snprintf(type, 31, "%s[WARN]", BOLD_YELLOW);
    else if (info_type == LOG_STATUS_IN_PROGRESS) snprintf(type, 31, "%s[STATUS: IN_PROGRESS]", YELLOW);
    else snprintf(type, 24, "%s[STATUS: DONE]", GREEN);

    auto time = Game_time::get_real_time();  // Get std::time_t struct.
    char *current_time = ctime(&time);  // Transform to const char*.
    current_time[strlen(current_time) - 1] = '\0';  // Remove newline.

    unsigned int buffer_size = strlen(string) + strlen(current_time) + strlen(type) + 9;  // 9 extra format chars.
    char buffer[buffer_size];
    if (snprintf(buffer, buffer_size, "%s [%s] : %s", type, current_time, string) < 0)
    {
      fprintf(stderr, "LOG_ERROR WHEN FORMATTING STRING (SNPRINTF)!\nEXITING...\n");
      exit(ERROR_SNPRINTF);
    }
    fprintf(stdout, "%s\n%s", buffer, RESET);
    save_to_file(buffer);
    free(string);
    va_end(args);
  }

  void close_file()
  {
    if (file_ptr && fclose(file_ptr))
    {
      alert(LOG_WARN, "ERROR WHEN CLOSING FILE STREAM! LOG MAY BE CORRUPTED...");
    } else if (!file_ptr) alert(LOG_WARN, "FILE STREAM WAS NEVER OPENED! NOT CLOSING IT...");
  }
}