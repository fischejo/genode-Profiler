  set log_dir "log/"
  set prefix [clock format [clock seconds] -format "%y-%m-%d-%H:%M:%S"]
  set log_file "$prefix.log"
  set tool_dir "${genode_dir}/../genode-Profiler/tool/"
  set stat_file "$prefix.stat"
  set plot_file "$prefix.svg"


proc profiler_enable_log {} {
  global log_dir log_file

  exec mkdir -p $log_dir
  log_file "$log_dir/$log_file"
}

proc profiler_postprocess {} {
  global log_dir log_file tool_dir stat_file plot_file 
  # parse log file and extract all statistic data
  exec ${tool_dir}profiler-filter ${log_dir}${log_file} ${log_dir}${stat_file}

  # create plot from statistic data
  exec ${tool_dir}profiler-plot --show --input ${log_dir}${stat_file} --output ${log_dir}${plot_file}
}


