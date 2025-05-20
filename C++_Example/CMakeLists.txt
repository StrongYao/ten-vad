# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name
project(TENVAD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# Include paths
include_directories(
  ./source/wave_io/
)

# List all header files explicitly
set(HEADER_FILE
  ../include/ten_vad.h
  ./wave_io/audio_wave.h
)

# List all source files explicitly
set(SOURCE_FILE
  ./ten_vad_example.cc
  ./wave_io/audio_wave.c
)

set(LIBS_PATH ../lib/)

# Create the executable
add_executable(ten-vad-example ${SOURCE_FILE} ${HEADER_FILE})
if(APPLE)
    get_filename_component(ABSOLUTE_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lib/macOS" ABSOLUTE)
    message("Using absolute path: ${ABSOLUTE_LIB_PATH}")
    # find_library(TEN_VAD_FRAMEWORK ten_vad.framework REQUIRED PATHS ${LIBS_PATH}/macOS)
    find_library(TEN_VAD_FRAMEWORK
                 NAMES ten_vad
                 REQUIRED
                 PATHS ${ABSOLUTE_LIB_PATH}
                 NO_DEFAULT_PATH)
    target_link_libraries(ten-vad-example PRIVATE ${TEN_VAD_FRAMEWORK})
elseif(WIN32)
    if(CMAKE_CL_64)
        set(LIBRARY_DIR ${LIBS_PATH}/Windows/x64)
    else()
        set(LIBRARY_DIR ${LIBS_PATH}/Windows/x86)
    endif()
    set(LIBRARY_PATH ${LIBRARY_DIR}/ten_vad.dll)
    set(LIBRARY_IMPORT_PATH ${LIBRARY_DIR}/ten_vad.lib)
    add_library(ten_vad SHARED IMPORTED)
    set_target_properties(ten_vad PROPERTIES
        IMPORTED_LOCATION ${LIBRARY_PATH}
        IMPORTED_IMPLIB ${LIBRARY_IMPORT_PATH}
    )
    target_link_libraries(ten-vad-example PRIVATE ten_vad)
endif()