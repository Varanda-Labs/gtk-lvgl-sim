cmake_minimum_required(VERSION 3.12.4)

include(sources-files.cmake)

set(NAME "lvgl-sim")
project(${NAME} LANGUAGES C)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk3)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

add_executable(${NAME} 
    ${SIM_FILES}
    ${APP_FILES})

target_link_libraries(${NAME} 
    PRIVATE 
    PkgConfig::GTK
    lvgl)

include_directories( ${CMAKE_SOURCE_DIR})

add_subdirectory(lvgl-build)
