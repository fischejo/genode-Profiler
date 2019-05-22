
#include <util/profiler.h>

/* define a static pointer to a timer session. This is used when calling
 * PROFILE_SCOPE and PROFILE_FUNCTION.
 */
Timer::Connection *_static_timer = nullptr;

