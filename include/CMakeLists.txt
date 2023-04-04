cmake_minimum_required(VERSION 3.25)
project(emeralddb VERSION 1.0.0 LANGUAGES C CXX)

# set the C++ standard 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# place binaries and libraries according to GNU standards 
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# gcov is required if we wanna generate project's coverage test report 
find_program(GCOV_PATH gcov)

if(NOT GCOV_PATH)
    message(FATAL_ERROR "Code coverage analysis requires gcov!")
else(NOT GCOV_PATH) 
    message("Gcov path: "${GCOV_PATH})
endif()

add_subdirectory(stx)
#enable_testing()

