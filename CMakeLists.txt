cmake_minimum_required(VERSION 3.12.4)

include(sources-files.cmake)

set(NAME "lvgl-sim")
project(${NAME} LANGUAGES C)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

add_executable(${NAME} 
    ${SIM_FILES}
    ${APP_FILES})

target_link_libraries(${NAME} 
    PRIVATE 
    lvgl
    ${GTK_LIBRARIES})

include_directories( ${CMAKE_SOURCE_DIR})

add_subdirectory(lvgl-build)
