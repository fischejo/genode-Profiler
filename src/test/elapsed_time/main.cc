#include <base/component.h>
#include <timer_session/connection.h>
#include <base/log.h>
#include <base/sleep.h>
#include <rm_session/connection.h>
#include <region_map/client.h>

/* Profiler include */
#include <util/profiler.h>


Genode::size_t Component::stack_size() { return 16*1024; }

void Component::construct(Genode::Env &env)
{

	using namespace Genode;
	Timer::Connection timer(env);
    for( int a = 0; a < 10000; a = a + 1 )
    {
      PROFILE_SCOPE("elapsed_time", "lightblue", timer);
      {
        PROFILE_SCOPE("measured_profile_function", "lightblue", timer);
      }
    }

    // trigger genode to terminate qemu
    log("the_end");        
    Genode::sleep_forever();
}
