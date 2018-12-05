/*
 * \brief  Timer class for printing elipsed time to log output.
 * \author Johannes Fischer
 * \date   2018-10-11
 */

#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <base/log.h>
#include <timer_session/connection.h>

class Profiler
{
 private:

  Timer::Connection &_timer;
  unsigned long _start;
  unsigned long _stop;  
  const char* _name;
  const char* _color;
 public:

 Profiler(const char* name,const char* color, Timer::Connection &timer)
   :
  _timer(timer),_name(name),_color(color)
  {
    _start = timer.now_us();
  }

  ~Profiler()
    {
      _stop = _timer.now_us();
      Genode::raw("\\STATS\\{",
                  "\"name\": \"", _name, "\", ",
                  "\"color\": \"", _color, "\", ",                  
                  "\"start\": ", _start, ", ",
                  "\"stop\": ", _stop,
                  "}\\STATE\\");
    }
};

#define PROFILE_SCOPE(name, color, timer) Profiler _pfinstance(name, color, timer);
#define PROFILE_FUNCTION(color, timer) Profiler _pfinstance(__PRETTY_FUNCTION__, color, timer)

#endif /* _PROFILER_H_ */
