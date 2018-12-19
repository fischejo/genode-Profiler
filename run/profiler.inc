
# global configuration
set log_dir [run_dir]/
set file_prefix [clock format [clock seconds] -format "%y-%m-%d-%H:%M:%S"]
set tool_dir "${genode_dir}/repos/genode-Profiler/tool/"
set show_plot true

if { [have_include "power_on/qemu"]} {
    rename run_power_on real_run_power_on
    rename run_power_off real_run_power_off
}

proc run_power_on {} {
    global log_dir file_prefix

    exec mkdir -p $log_dir
    log_file "$log_dir/$file_prefix.log"

    real_run_power_on
}


proc run_power_off {} {
    global log_dir tool_dir file_prefix show_plot

    # parse log file and extract all statistic data
    exec ${tool_dir}profiler-filter ${log_dir}${file_prefix}.log ${log_dir}${file_prefix}.profile

    # build argument string for profiler-plot
    # --input
    set profiler_args "--input ${log_dir}${file_prefix}.profile"

    # (optional) --show
    if {$show_plot} {
        append profiler_args " --show"
    }

    # --output *.{png, svg, pdf}
    append profiler_args " --output ${log_dir}${file_prefix}.png"
    append profiler_args " --output ${log_dir}${file_prefix}.svg"
    append profiler_args " --output ${log_dir}${file_prefix}.pdf"    

    # create plot
    eval exec ${tool_dir}profiler-plot $profiler_args
    
	real_run_power_off
}
