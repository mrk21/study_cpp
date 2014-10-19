cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(StudyCPP)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION LESS "3.5")
    message(FATAL_ERROR "A supported Clang version is equal to or greater than 3.5!")
  endif()
else()
  message(FATAL_ERROR "Supported compiler is Clang!")
endif()

find_package(Boost 1.56 REQUIRED system thread)

set(CMAKE_CXX_FLAGS "-std=gnu++14 -stdlib=libc++ -Wall -Wextra")

file(GLOB_RECURSE StudyCPP_SOURCES "*.cpp")

foreach(_path ${StudyCPP_SOURCES})
  get_filename_component(_name ${_path} NAME_WE)
  set(_bin_name "${_name}")
  add_executable(${_bin_name} ${_path})
  target_link_libraries(${_bin_name} ${Twit4v_LIBRARY_NAME})
endforeach()
