cmake_minimum_required(VERSION 3.12)

project(TrackerBoyProject C)

# Set the C standard to C17
set(CMAKE_C_STANDARD 17)

# Add the include directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create a list of source files in the src folder
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

# Add the main.c file to the source list
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/trackerBoy/main.c)

# Add the executable and link the sources
add_executable(TrackerBoy ${SOURCES})
