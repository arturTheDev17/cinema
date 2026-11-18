# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CinemaMecha_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CinemaMecha_autogen.dir\\ParseCache.txt"
  "CinemaMecha_autogen"
  )
endif()
