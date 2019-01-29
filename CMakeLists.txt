cmake_minimum_required(VERSION 3.0.0)
project(CLI-proto VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

IF(MSVC)
  add_compile_options(/permissive-)
ENDIF()
    
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

find_package(range-v3 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
include(CTest)
include(Catch)

file(GLOB test_files test/*.cpp)
foreach(filename ${test_files})
  get_filename_component(target ${filename} NAME_WE)
  add_executable(${target} ${filename})
  target_link_libraries(${target} Catch2::Catch2 range-v3)
  catch_discover_tests(${target})
endforeach(filename)


