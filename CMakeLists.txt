cmake_minimum_required(VERSION 3.12)

project(TrackerBoyProject C)

# Set the C standard to C17
set(CMAKE_C_STANDARD 17)

# Add the include directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create a list of source files in the src folder
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

# Build a shared library from the source files
add_library(TrackerBoyLib SHARED ${SOURCES})

# Add the LOG_USE_COLOR compiler flag
add_definitions(-DLOG_USE_COLOR)

# Add the main.c file to the source list
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/trackerBoy/main.c)

# Add the executable and link the sources with the shared library
add_executable(TrackerBoy ${SOURCES})
target_link_libraries(TrackerBoy PRIVATE TrackerBoyLib)
