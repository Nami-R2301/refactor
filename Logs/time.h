//
// Created by nami on 5/11/22.
//

#pragma once

#include <GLFW/glfw3.h>  // GLFW
#include <chrono>  // time_t

#define NANO 1000000000.0f

using namespace std::chrono;

class Game_time
{
public:
  static time_t get_real_time();
  float get_delta_time_f() const;
  duration<long, std::ratio<1, 1000000000>> get_delta_time_chrono() const;

  time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>>
  get_previous_time() const;
  void set_previous_time(const time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>
  > &clock_struct);
  time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>>
  get_current_time() const;
  void set_current_time(const time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>
  > &clock_struct);
private:
  time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>>
  previous_time;
  time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>>
  current_time;
};
