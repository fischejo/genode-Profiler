
PROFILER_INSTALL_DIR := $(BUILD_BASE_DIR)/tool/profiler
PROFILER_TOOL_DIR := $(REP_DIR)/tool

HOST_TOOLS += profiler-merge profiler-filter profiler-plot profilerlib.py

profiler%: $(PROFILER_INSTALL_DIR)/$@
	@echo "    INSTALL " $(PROFILER_INSTALL_DIR)/$@
	$(VERBOSE)mkdir -p $(PROFILER_INSTALL_DIR)
	$(VERBOSE)cp $(PROFILER_TOOL_DIR)/$@ $(PROFILER_INSTALL_DIR)/$@

