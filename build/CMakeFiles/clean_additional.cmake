# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/qcustomplot_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/qcustomplot_autogen.dir/ParseCache.txt"
  "qcustomplot_autogen"
  "src/CMakeFiles/ethercat_core_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/ethercat_core_autogen.dir/ParseCache.txt"
  "src/CMakeFiles/ethercat_monitor_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/ethercat_monitor_autogen.dir/ParseCache.txt"
  "src/ethercat_core_autogen"
  "src/ethercat_monitor_autogen"
  "third_party/SOEM/CMakeFiles/soem_autogen.dir/AutogenUsed.txt"
  "third_party/SOEM/CMakeFiles/soem_autogen.dir/ParseCache.txt"
  "third_party/SOEM/soem_autogen"
  )
endif()
