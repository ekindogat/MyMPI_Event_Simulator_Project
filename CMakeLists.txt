cmake_minimum_required(VERSION 3.10)
project(mpi_parallel_sort_simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/event_simulator.cpp
    src/my_mpi.cpp
    src/processor.cpp
)

# Add header files
set(HEADERS
    lib/event_simulator.hpp
    lib/my_mpi.hpp
    lib/processor.hpp
    lib/event_types.hpp
    lib/utils.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE lib)

# Add compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif() 