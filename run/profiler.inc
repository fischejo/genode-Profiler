set run_dir [run_dir]
set log_dir "$run_dir/log/"
set prefix [clock format [clock seconds] -format "%y-%m-%d-%H:%M:%S"]
set log_file "$prefix.log"
set tool_dir "${genode_dir}/../genode-Profiler/tool/"
set stat_file "$prefix.stat"
set profiler_show_plot true


rename run_power_on real_run_power_on
proc run_power_on {} {
    global log_dir log_file

    exec mkdir -p $log_dir
    log_file "$log_dir/$log_file"

    real_run_power_on
}

rename run_power_off real_run_power_off
proc run_power_off {} {
    global log_dir log_file tool_dir stat_file prefix profiler_show_plot

    # parse log file and extract all statistic data
    exec ${tool_dir}profiler-filter ${log_dir}${log_file} ${log_dir}${stat_file}

    # build argument string for profiler-plot
    # --input
    set profiler_args "--input ${log_dir}${stat_file}"

    # (optional) --show
    if {$profiler_show_plot} {
        append profiler_args " --show"
    }

    # --output *.{png, svg, pdf}
    append profiler_args " --output ${log_dir}${prefix}.png"
    append profiler_args " --output ${log_dir}${prefix}.svg"
    append profiler_args " --output ${log_dir}${prefix}.pdf"    

    # create plot
    eval exec ${tool_dir}profiler-plot $profiler_args
    
	real_run_power_off
}


