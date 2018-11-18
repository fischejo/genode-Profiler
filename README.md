# Function Profiler based on Serial Log Output for Genode 

![Screenshot](doc/plotted.png)

## OS Requirements

Pythons `matplotlib` is required.

```bash
sudo apt-get install python3-pip
sudo pip3 install matplotlib
```

## Application Integration

1. Include `<util/profiler.h>` in your Application
2. Create a Timer Session and make it available in every function which should
   be profiled.

3. Every function, which should be profiled requires the macro
   `PROFILE_FUNCTION(color, timer)` at the beginning. The attribute `color` is a
   string like `blue`, and `timer` is the timer instance.

4. In order to profile a code area, put this code in a seperate Scope and
   include `PROFILE_SCOPE(name, color, timer)` in the beginning of the
   scope. The variable name is a string and represents the measurement in the
   plot. Example:
   ```C++
   {
     PROFILE_SCOPE("simple-print", "yellow", _timer)
	 Genode::log("Hello World")
   }
   ```

5. (Optional) If you want to use automatic logging, filtering and plotting of
   your profiling data whenever your application runs in Qemu, source
   `profiler.inc`.
   ```
   source ${genode_dir}/../genode-Profiler/run/profiler.inc
   ```
   After running the run file, a GUI with the plot graph opens. You can disable
   this behaviour with `set profiler_show_plot false`.
   The plot is automatically exported as `.svg`, `.png`, `.pdf`. All files are
   in your build directory under `var/run/.../`.

## Genode Integration

1. Clone this repository to the root directory of your Genode.

2. Add this repository to the variable REPOSITORIES in `build.conf`.
   ```
   REPOSITORIES += $(GENODE_DIR)/../genode-Profiler
   ```


## Further documentation
Further documentation is in directory `doc`.

* [Implementation](./doc/implementation.md)
* [Summary Profiling Techniques](./doc/profiler_techniques.md)

## Further Links

* [CheckpointRestore-Statistics](https://github.com/pecheur/CheckpointRestore-Statistics)
* [genode-CheckpointRestore-SharedMemory](https://github.com/pecheur/genode-CheckpointRestore-SharedMemory)


