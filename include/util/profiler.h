/*
 * \brief  Timer class for printing elipsed time to log output.
 * \author Johannes Fischer
 * \date   2018-10-11
 */

#ifndef _PROFILER_SCOPE_H_
#define _PROFILER_SCOPE_H_

#include <base/log.h>
#include <timer_session/connection.h>

namespace Profiler {
  class Scope;
}
extern Timer::Connection *_static_timer;

class Profiler::Scope
{
 private:
  Timer::Connection *_timer;
  unsigned long _start;
  unsigned long _stop;  
  const char* _name;
  const char* _color;
 public:

 Scope(const char* name,const char* color, Timer::Connection *timer)
   :
  _timer(timer),_name(name),_color(color)
  {
    if(_timer)
      _start = _timer->elapsed_ms()*1000;
    else
      Genode::log("NOOOOOOO TIMER FOUND");
  }

  ~Scope()
    {
      if(_timer) {
	_stop = _timer->elapsed_ms()*1000;
	Genode::raw("\\STATS\\{",
		    "\"name\": \"", _name, "\", ",
		    "\"color\": \"", _color, "\", ",                  
		    "\"start\": ", _start, ", ",
		    "\"stop\": ", _stop,
		    "}\\STATE\\");
      }
    }
};


/* In order to use following macros, a static timer conenction need to be initialized:
 * 1. Add the library `profiler` to the `LIBS` variable
 * 2. Initialize a `Timer::Connection timer;` object
 * 3. Set the static timer: `PROFILER_INIT(&timer);`
 */
#define PROFILER_INIT(timer) _static_timer = timer;
#define PROFILE_SCOPE(name, color) Profiler::Scope _pfinstance(name, color, _static_timer);
#define PROFILE_FUNCTION(color) Profiler::Scope _pfinstance(__PRETTY_FUNCTION__, color, _static_timer)

#endif /* _PROFILER_SCOPE_H_ */
