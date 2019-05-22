
PROFILER_HOST_DIR := $(BUILD_BASE_DIR)/tool/profiler
PROFILER_TOOL_DIR := $(REP_DIR)/tool

HOST_TOOLS += profiler-merge profiler-filter profiler-plot profilerlib.py

profiler%: $(PROFILER_HOST_DIR)/$@
	@echo "    INSTALL " $(PROFILER_HOST_DIR)/$@
	$(VERBOSE)mkdir -p $(PROFILER_HOST_DIR)
	$(VERBOSE)cp $(PROFILER_TOOL_DIR)/$@ $(PROFILER_HOST_DIR)/$@

