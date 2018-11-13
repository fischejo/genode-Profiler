# Software-based Function Profiler

This documentation introduces a software-based profiler with serial log
communication. Further information about other profiling techniques are
summarized in [profiler\_techniques.md](profiler_techniques.md)

## Time Source

In order to measure elipsed time of a function, a source of time is
required. Following might be possible:
1. RTC
2. `Trace::timestamp()` method
   Following should be supported, but the qemu instances hangs up, when it is used:
   ```
   #include <trace/timestamp=.h>
   Trace::timestamp= now  = Trace::timestamp=();
   ```

3. `Timer_session_component::now_us()`
   This is the used time source by the Profiler

## Implementation

## Creating profiler log

The profiling functionality is implemented in class `Profiler`
(`include/rtcr/util/profiler.h`). It depends on a Genode::Timer
session. Therefore several adjustments are done in `checkpointer.h`,
`rtcr_restore_child/main.cc` in order to passthrough the timer session to the
profiler class.

In order to profile a function, add `PROFILE_FUNCTION(_timer)` to the top of
your target function. A object of `Profiler` is created and the elapsed time of
the timer session is stored. When the scope of the object ends (in this case,
the function scope), `~Profiler` is called. The time is measured another time
and both timestamps are printed to the serial log:

```
\STATS\{"name": "Genode::List<Rtcr::Kcap_badge_info> Rtcr::Checkpointer::_create_kcap_mappings()", "start": 3932000, "stop": 3941000}\STATE\
```

Every profile log starts with  `\STATS\` and ends with `\STATE\`. It includes a
json string with following attributes: `name`, `start` and `stop`.

## Extracing profiler log and plotting

The except script `run/profiler_rtcr_restore_child.run` is extended and calls
`profiler-filter` and `profiler-plot` after a successful execution of
qemu. Both scripts are in `./tool/`.

* **tool/profiler-filter** extracts profiler data from a log stream.
* **tool/profiler-plot** is a python script and creates a plot from profiler data.

Log files are exported to `${BUILD_DIR}/log/`. This directory contains:
* `.log` raw log from serial output
* `.stat` extracted profiler data from `.log`
* `.png` plotted graph of profiled functions



