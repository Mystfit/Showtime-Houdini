cmake_minimum_required(VERSION 3.13.0)

project(ShowtimeHoudini VERSION 0.0.1)

#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")

find_package(showtime CONFIG REQUIRED)
find_package(houdini CONFIG REQUIRED)

set(SHOWTIME_HOUDINI_TARGET ${PROJECT_NAME})
add_library(${SHOWTIME_HOUDINI_TARGET} SHARED "")
include(src/CMakeLists.txt)
houdini_configure_target( ${SHOWTIME_HOUDINI_TARGET} )
