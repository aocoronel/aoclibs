cmake_minimum_required(VERSION 3.10)
project(aoclibs C)

project (aoclibs
  VERSION 0.1.0
  DESCRIPTION "Coronel's C Libraries"
  LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)
function(enable_strict_warnings target)
    if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE /W4 /WX)
    endif()
endfunction()

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library(printh SHARED info/printh.c)
add_library(printfc SHARED info/printfc.c)
add_library(bashgen SHARED automation/bashgen.c)
add_library(zshgen SHARED automation/zshgen.c)
add_library(heap_alloc SHARED heap/heap_alloc.c)
add_library(heap_string SHARED heap/heap_string.c)

target_include_directories(printh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/info)
target_include_directories(printfc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/info)
target_include_directories(bashgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/automation)
target_include_directories(zshgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/automation)
target_include_directories(heap_alloc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/heap)
target_include_directories(heap_string PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/heap)

set_target_properties(printh printfc bashgen zshgen heap_alloc heap_string PROPERTIES
    VERSION 0.1.0
    SOVERSION 1
)

enable_strict_warnings(printh printfc bashgen zshgen heap_alloc heap_string)

install(TARGETS printh printfc bashgen zshgen heap_alloc heap_string
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/aoclibs
)

foreach(dir IN ITEMS info automation heap)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/
        DESTINATION include/aoclibs
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*.c" EXCLUDE
    )
endforeach()
