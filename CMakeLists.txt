cmake_minimum_required(VERSION 3.10)
project(CAS_Calculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Find SFML (optional)
find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
if(SFML_FOUND)
    message(STATUS "SFML found - GUI grapher will be available")
    add_definitions(-DSFML_AVAILABLE)
else()
    message(STATUS "SFML not found - only console grapher will be available")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Parser library
add_library(parser_lib
    parser/ExpressionParser.cpp
)

# Symbolic engine library
add_library(symbolic_lib
    cas/SymbolicEngine.cpp
)
target_link_libraries(symbolic_lib parser_lib)

# Console Grapher library (no external dependencies)
add_library(console_grapher_lib
    grapher/ConsoleGrapher.cpp
)
target_link_libraries(console_grapher_lib parser_lib symbolic_lib)

# SFML Grapher library (requires SFML)
if(SFML_FOUND)
    add_library(grapher_lib
        grapher/Grapher.cpp
    )
    target_link_libraries(grapher_lib parser_lib symbolic_lib sfml-graphics sfml-window sfml-system)
endif()

# Test executable
add_executable(test_parser test_parser.cpp)
target_link_libraries(test_parser parser_lib)

# Symbolic engine test
add_executable(test_symbolic test_symbolic.cpp)
target_link_libraries(test_symbolic symbolic_lib)

# Symbolic engine demo
add_executable(demo_symbolic demo_symbolic.cpp)
target_link_libraries(demo_symbolic symbolic_lib)

# Advanced features test
add_executable(test_advanced test_advanced.cpp)
target_link_libraries(test_advanced symbolic_lib)

# Comprehensive CAS demo
add_executable(cas_demo cas_demo.cpp)
target_link_libraries(cas_demo symbolic_lib)

# Console Grapher test (no SFML required)
add_executable(test_console_grapher test_console_grapher.cpp)
target_link_libraries(test_console_grapher console_grapher_lib)

# Integrated CAS Grapher demo
add_executable(cas_grapher_demo cas_grapher_demo.cpp)
target_link_libraries(cas_grapher_demo console_grapher_lib)

# SFML Grapher test (requires SFML)
if(SFML_FOUND)
    add_executable(test_grapher test_grapher.cpp)
    target_link_libraries(test_grapher grapher_lib)
endif()

# Main CAS executable (placeholder for future)
add_executable(cas_main main.cpp)
target_link_libraries(cas_main parser_lib)

# Interactive CAS
add_executable(interactive_cas interactive_cas.cpp)
target_link_libraries(interactive_cas symbolic_lib)

if(SFML_FOUND)
    target_link_libraries(interactive_cas grapher_lib)
else()
    target_link_libraries(interactive_cas console_grapher_lib)
endif()

# Set output directories
set_target_properties(test_parser cas_main
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Optional: Enable testing
enable_testing()
add_test(NAME ParserTest COMMAND test_parser)
