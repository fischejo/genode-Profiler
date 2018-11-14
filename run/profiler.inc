set run_dir [run_dir]
set log_dir "$run_dir/log/"
set prefix [clock format [clock seconds] -format "%y-%m-%d-%H:%M:%S"]
set log_file "$prefix.log"
set tool_dir "${genode_dir}/../genode-Profiler/tool/"
set stat_file "$prefix.stat"
set plot_file "$prefix.svg"


rename run_power_on real_run_power_on
proc run_power_on {} {
    global log_dir log_file

    exec mkdir -p $log_dir
    log_file "$log_dir/$log_file"

    real_run_power_on
}


rename run_power_off real_run_power_off
proc run_power_off {} {
    global log_dir log_file tool_dir stat_file plot_file

    # parse log file and extract all statistic data
    exec ${tool_dir}profiler-filter ${log_dir}${log_file} ${log_dir}${stat_file}

    # create plot from statistic data
    exec ${tool_dir}profiler-plot --show --input ${log_dir}${stat_file} --output ${log_dir}${plot_file}

    
	real_run_power_off
}


