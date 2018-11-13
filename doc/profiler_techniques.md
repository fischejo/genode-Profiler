# Profiling Techniques

This document summarizes options for measuering execution time of functions and
code parts in the checkpoint/restore component.


# Available Tools

* DTrace
  * Solaris, Linux, BSD, Mac OS X 
  * Free/open source (CDDL) 
* dynamoRIO by RIO 
  * Linux, Windows 
  * Dynamic binary instrumentation framework for the development of dynamic
    program analysis tools. 
  * Free/open source - BSD 
* gprof
  *	Linux/Unix 
  * Any language supported by gcc
  * Free/open source - BSD version is part of 4.2BSD and GNU version is part of
    GNU Binutils (by GNU Project) 
* Systemtap
  * Linux
  * open source
* valgind
  * Linux, OSX, Solaris, Android 
  * Any, including assembler 
  * Free/open source (GPL) 

All tools can not be used, as there is no libc, no Linux perf API or any similar
API.


# Techniques
1. software patching with time measure instructions
   1. Manual function patching
	  implemented in [Profiler](profiler.md)
   2. GCC Function Instrumentation
	  * require C-Code
	  * hangs
	  * maybe due to recursive loop
	  * excluding functions didn't work
	  * [manual](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html)
   
2. GDB Monitor
   * poor man's sampling profiler
   * [Explaination](https://stackoverflow.com/questions/375913/how-can-i-profile-c-code-running-on-linux)
   * [Script](https://poormansprofiler.org/)
   * Problem: no ROM dataspace

3. Fiasco Kernel Debugger (jdb)
   * has Performance Counters for kernel & userspace
   * controlable from userspace
   * [dir](/home/fischejo/university/informatik-master/idp/00_ressources/Fiasco.OC/fiasco_kernel_debugger_manual.pdf)
   * L4 needs enabled debugger  

4. Genode TRACE API
   * Info about total, recent execution time of threads
   * according to code:  trace of signals, RPC möglich
   * more for Threads than for functions
  
5. DSTREAM + Watchpoints counting? DSTREAM Extension?
   * [Software Breakpoints](https://web.archive.org/web/20130605011618/http://www.technochakra.com:80/software-breakpoints/)
   
6. Genode Report Session
   * Store information in Report Session
   * Query via other process
   * uses XML

# Information about Tracing, Logging and Profiling

## Fiasco.OC JDB
Although developed and traditionally packaged together with Fiasco, the Fiasco
Kernel Debugger is not part of the Fiasco µ-kernel. Fiasco can run without JDB
or with another kernel debugger (see compile options).  There is no other
connection between Fiasco and JDB that JDB intimately knows Fiasco’s data
structures.
  
## Genode 
* [Reference](https://genode.org/documentation/developer-resources/gdb)
* [GDB Monitor graphic in slides](/home/fischejo/university/informatik-master/idp/00_ressources/Genode/nfeske-genode-fosdem-2013-02.pdf)

### L4 Kernel Debugger 
Most L4 kernels come with built-in kernel debuggers that allow the inspection of
kernel objects such as threads and address spaces. This way, we get a global
view on the system from the kernel's perspective. For example, the mapping of
virtual memory to physical pages can be revealed, the communication
relationships between threads become visible, and the ready queue of the
scheduler can be observed. To a certain extend, kernel debuggers had been
complemented with useful facilities for debugging **user-level programs**. For
example, the Fiasco kernel debugger comes with a convenient **backtrace**
function that parses the call stack of a given thread. Using the addresses
printed in the backtrace, the corresponding code can be matched against the
output of the objdump utility that comes with the GNU binutils. Among the kernel
debuggers of Genode's supported base platforms, the variants of L4/Fiasco and
respectively Fiasco.OC stand out. We often find ourself switching to one of
these kernel platforms when we hit a hard debugging problem for the sole reason
that the kernel debugger is so powerful. 

However, with complex applications, the kernel debugger becomes **awkward to
use**. For example, if an application uses shared libraries, the kernel has no
interpretation of them. Addresses that appear as the backtrace of the stack must
be manually matched against the loading addresses of the individual shared
libraries, the objdump must be used with the offset of the return address from
the shared-library's base address. Saying that this process is inconvenient
would be a blatant understatement.
  
### GDB & Qemu
For problems that call for source-level debugging and single-stepping, however,
we have found Qemu's GDB stub extremely useful. This stub can be used to attach
GDB to a virtual machine emulated by Qemu. By manually loading symbols into GDB,
this approach can be used to perform source-level debugging to a certain
degree. However, there are a number of restrictions attached to this
solution. First, Qemu is not aware of any abstractions of the running operating
system. So if the kernel decides to preempt the current thread and switch to
another, the single-stepping session comes to a surprising end. Also, Qemu is
not aware of the different address spaces. Hence, a breakpoint triggers as soon
as the program counter reaches the breakpoint address regardless of the
process. If multiple applications use the same virtual addresses (which is
usually the case), we get an aliasing problem. This problem can be mitigated by
linking each application to a different virtual-address range. **However, this
effort is hardly recommendable** as a general solution. Still, Qemu's GDB stub can
save the soul of a developer who has to deal with problems in the category of
low-level C++ exception handling.
  
### GDB + base-linux
For debugging higher-level application code and protocols, using GDB on Linux is
a viable choice if the application scenario can executed on the base-linux
platform. For many problems on Genode, this is apparently the case because most
higher-level code is platform-independent. On the Linux base platform, each
Genode process runs as an individual Linux process. Consequently, GDB can be
attached to such a process using the gdb -p command. To synchronize the point in
time of attaching GDB, the utility function wait_for_continue provided by the
Linux version of Genodes env library can be utilized. In general, this approach
is viable for high-level code. There are even success stories with debugging the
program logic of a Genode device driver on Linux even though no actual hardware
has been present the Linux platform. However, also this approach has severe
limitations (besides being restricted to the base-linux platform). The most
prevalent limitation is the lack of thread debugging. For debugging normal Linux
applications, GDB relies on certain glibc features (e.g., the way of how threads
are managed using the pthread library and the handling of thread-local
storage). **Since, Genode programs are executed with no glibc, GDB lacks this
information.**

### GDB monitor concept
GDB monitor is a Genode process that sits in-between the target and its normal
parent. As the parent of the target it has full control over all interactions of
the target with the rest of the system. I.e., all session requests originating
from the target including those that normally refer to core's services are first
seen by GDB monitor. GDB monitor, in turn, can decide whether to forward such a
session request to the original parent or to virtualize the requested service
using a local implementation. The latter is done for all services that GDB
monitor needs to inspect the target's address space and thread state. In
particular, GDB monitor provides local implementations of the CPU and RM (and
ROM) services. Those local implementations use real core services as their
respective backend and a actually mere wrappers around the core service
functions. However, by having the target to interact with GDB monitor instead of
core directly, GDB monitor gains full control over all threads and memory
objects (dataspace) and the address space of the target. All session requests
that are of no specific interest to GDB monitor are just passed through to the
original parent. This way, the target can use services provided by other Genode
programs as normally. Furthermore, service announcements of the target are
propagated to the original parent as well. This way, the debugging of Genode
services becomes possible.

Besides providing a virtual execution environment for the target, the GDB
monitor contains the communication protocol code to interact with a remote GNU
debugger. This code is a slightly modified version of the so-called gdbserver
and uses a Genode terminal session to interact with GDB running on the
host. From GDB monitor's point of view, the terminal session is just a
bidirectional line of communication with GDB. The actual communication mechanism
depends on the service that provides the terminal session on Genode. Currently,
there are two services that can be used for this purpose: TCP terminal provides
terminal sessions via TCP connections, and Genode's UART drivers provides one
terminal session per physical UART. Depending on which of those terminal
services is used, the GDB on the host must be attached either to a network port
or to a comport of the target, i.e. Qemu


### No simulation of read-only memory

The current implementation of GDB monitor hands out only RAM dataspaces to the
target. If the target opens a ROM session, the ROM dataspace gets copied into a
RAM dataspace. This is needed to enable GDB monitor to patch the code of the
target. Normally, the code is provided via read-only ROM dataspace. So patching
won't work. The current solution is the creation of a RAM copy.

However, this circumstance may have subtle effects on the target. For example a
program that crashed because it tries to write to its own text segment will
behave differently when executed within GDB monitor.  

### CPU register state during system calls

When intercepting the execution of the target while the target performs a system
call, the CPU register state as seen by GDB may be incorrect or incomplete. The
reason is that GDB monitor has to retrieve the CPU state from the kernel. Some
kernels, in particular Fiasco.OC, report that state only when the thread crosses
the kernel/user boundary (at the entry and exit of system calls or then the
thread enters the kernel via an exception). For a thread that has already
entered the kernel at interception time, this condition does not apply. However,
when stepping through target code, triggering breakpoints, or intercepting a
busy thread, the observed register state is current.  

### No support for watchpoints

The use of watchpoints is currently not supported. This feature would require
special kernel support, which is not provided by most kernels used as base
platforms of Genode.  

### Memory consumption

GDB monitor is known to be somehow lax with regard to consuming memory. Please
don't be shy with over-provisioning RAM quota to gdb_monitor.


## Genode: Light-weight event tracing

### [Topic: Dic Driver Performance](https://sourceforge.net/p/genode/mailman/message/30865209/)
* at the bottom of the thread

> 3. What is the suggested "best" way to debug IPC and profile applications?

We used to use the Fiasco.OC trace buffer for that, in particular the
IPC-tracing feature. But admittedly, it is quite difficult to map the
low-level information gathered by the kernel to the high-level
application view. E.g., the kernel stores the kernel-internal names of
kernel objects, which are unrelated to how the kernel objects are named
at the user land. So looking at the trace buffer is confusing. Not to
speak of interpreting RPC protocols - that is hardly feasible for a work
load of modest complexity.

Fortunately, I can report that there is work under way to equip Genode with a
tracing infrastructure, which will allow us to gather traces about RPCs
(including the actual RPC function names), contended locks, and signals. The new
facility is currently developed by Josef Söntgen (**cnuke** at GitHub). We plan
to include it in **Genode 13.08**.

Cheers
Norman

### Genode 13.08 Release Notes
* [13.08](https://genode.org/documentation/release-notes/13.08#Light-weight_event_tracing)
* [13.11](https://genode.org/documentation/release-notes/13.11#Improved_event_tracing)
  * base/include/base/trace/buffer.h
  * os/src/test/trace/

* [15.08](https://genode.org/documentation/release-notes/15.08#Enhanced_tracing_facilities)
  * os/src/app/trace_subject_reporter

The TRACE session interface is located at base/include/trace_session/. A simple
example for using the service is available at os/src/test/trace/ and is
accompanied with the run script os/run/trace.run. The test demonstrates the
TRACE session interface by gathering a trace of a thread running locally in its
address space.
  
At the current stage, the feature is available solely on NOVA since this is our
kernel of choice for using Genode as our day-to-day OS. On all other base
platforms, the returned execution times are 0.
  
With the introduction of our light-weight event-tracing facility in version
13.08, we laid the foundation for such tools. The current release extends core's
TRACE service with the ability to obtain statistics about CPU utilization. More
specifically, it enables clients of core's TRACE service to obtain the execution
times of trace subjects (i.e., threads). The execution time is delivered as part
of the Subject_info structure. In addition to the execution time, the structure
delivers the information about the affinity of the subject with a physical CPU.

At the current stage, the feature is available solely on NOVA since this is our
kernel of choice for using Genode as our day-to-day OS. On all other base
platforms, the returned execution times are 0. To give a complete picture of the
system's threads, the kernel's idle threads (one per CPU) are featured as trace
subjects as well. Of course, idle threads cannot be traced but their
corresponding trace subjects allow TRACE clients to obtain the idle time of each
CPU.

By obtaining the trace-subject information in periodic intervals, a TRACE client
is able to gather statistics about the CPU utilization attributed to the
individual threads present (or no longer present) in the system. One instance of
such a tool is the new trace-subject reporter located at
os/src/app/trace_subject_reporter. It acts as a TRACE client, which delivers the
gathered trace-subject information in the form of XML-formatted data to a report
session. This information, in turn, can be consumed by a separate component that
analyses the data. In contrast to the low-complexity trace-subject reporter,
which requires access to the privileged TRACE services of core, the (potentially
complex) analysing component does not require access to core's TRACE service. So
it isn't as critical as the trace-subject monitor. The first representative of a
consumer of trace-subject reports is the CPU-load display mentioned in Section
CPU-load monitoring and depicted in Figure 3.

In addition to the CPU-monitoring additions, the tracing facilities received
minor refinements. Up to now, it was not possible to trace threads that use a
CPU session other than the component's initial one. A specific example is
VirtualBox, which employs several CPU sessions, one for each priority. This
problem has been solved by associating the event logger of each thread with its
actual CPU session. Consequently, the tracing mechanism has become able to trace
VirtualBox, which is pivotal for our further optimizations.
  
* [18.02](https://genode.org/documentation/release-notes/18.02#New_trace-logging_component)

The new trace-logger component can be used to easily gather, process, and export
different types of tracing data. Furthermore, it marks the next step towards a
user framework that makes access to Genode's manifold tracing abilities (13.08,
13.11, 15.08) intuitive and convenient.

### Genode 14.02 Trace FS
* [Link](https://genode.org/documentation/release-notes/14.02#Trace_file_system)

*  The new trace\_fs server provides access to a trace session by providing a
   file-system session as front end. Combined with Noux, it allows for the
   interactive exploration and tracing of Genode's process tree using
   traditional Unix tools.  Each trace subject is represented by a directory
   (thread_name.subject) that contains specific files, which are used to control
   the tracing process of the thread as well as storing the content of its trace
   buffer
   
### Genode 16.08 RElease Notes: Staticial profiling
* [Link](https://genode.org/documentation/release-notes/16.08#Statistical_profiling)
