cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
project(asciify)

add_executable(asciify
  src/main.cpp
)

set_property(TARGET asciify PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

find_path(OPENCV_INCLUDE
  NAMES opencv2
  HINTS ENV{CMAKE_PREFIX_PATH}
  PATH_SUFFIXES opencv/build/include
)
find_library(OPENCV_LIB
  NAMES opencv_world451d.lib
  HINTS ENV{CMAKE_PREFIX_PATH}
  PATH_SUFFIXES opencv/build/x64/vc15/lib
)

include_directories(${OPENCV_INCLUDE})
target_link_libraries(asciify ${OPENCV_LIB})
