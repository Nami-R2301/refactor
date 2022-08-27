//
// Created by nami on 5/16/22.
//

#pragma once

// Game_time
#include "time.h"

// strlen, std::string and snprintf.
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#define LOG_INFO 0
#define LOG_WARN 1
#define LOG_ERROR 2
#define LOG_STATUS_IN_PROGRESS 3
#define LOG_STATUS_DONE 4

#define ERROR_HEAP_ALLOC -11
#define ERROR_SNPRINTF -13

//the following are UBUNTU/LINUX, and macOS ONLY terminal color codes.
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define YELLOW  "\033[33m"      /* Yellow */
#define BOLD_YELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define GREEN   "\033[32m"      /* Green */
#define WHITE   "\033[37m"      /* White */

namespace Nami
{

  void open_file();
  void alert(int info_type = LOG_INFO, const char *format = "\n", ...);
  void save_to_file(const char *info);
  void print_logs();
  void reset_logs();
  void close_file();
}
