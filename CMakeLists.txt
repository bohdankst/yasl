# Encrypted Bootloader
cmake_minimum_required(VERSION 3.28)

project(YASL C)

set(YASL_ENABLE_UT OFF CACHE BOOL "Disable UT by default" FORCE)

add_subdirectory(src)

if (${YASL_ENABLE_UT})
    add_subdirectory(ut)
endif ()
