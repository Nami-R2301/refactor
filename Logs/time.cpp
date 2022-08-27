//
// Created by nami on 5/11/22.
//

#include "time.h"

time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>>

Game_time::get_previous_time() const
{
  return this->previous_time;
}

void
Game_time::set_previous_time(const time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>
> &previous_time_)
{
this->
previous_time = previous_time_;
}

time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>>

Game_time::get_current_time() const
{
  return this->current_time;
}

void
Game_time::set_current_time(const time_point <system_clock, duration<long, std::ratio < 1, 1000000000>>
> &current_time_)
{
this->
current_time = current_time_;
}

time_t Game_time::get_real_time()
{
  auto current_time = time(nullptr);
  return current_time;
}

float Game_time::get_delta_time_f() const
{
  auto delta = get_current_time() - get_previous_time();
  return (float) delta.count();
}

duration<long, std::ratio<1, 1000000000>> Game_time::get_delta_time_chrono() const
{
  return get_current_time() - get_previous_time();
}
