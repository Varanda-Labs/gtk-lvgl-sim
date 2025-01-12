cmake_minimum_required(VERSION 3.12.4)

include(sources-files.cmake)

set(NAME "lvgl-sim")
project(${NAME} LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(${NAME} 
    ${SIM_FILES}
    ${APP_FILES})

target_link_libraries(${NAME} 
    PRIVATE 
    PkgConfig::GTK4
    lvgl)

add_subdirectory(lvgl-build)
