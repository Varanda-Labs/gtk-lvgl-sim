cmake_minimum_required(VERSION 3.12.4)

include(sources-files.cmake)

set(NAME "lvgl-sim")
project(${NAME} LANGUAGES C)

# for production comment out the following line
if(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})
set(GRESOURCE_C   gresource.c)

add_executable(${NAME} 
    ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
    ${SIM_FILES}
    ${APP_FILES}
)

if(MUSIC_DEMO)
    message("*******************************")
    message("*                             *")
    message("* Build setup for MUSIC_DEMO  *")
    message("*                             *")
    message("*******************************")
    target_link_libraries(${NAME} 
        PRIVATE 
        lvgl
        demos
        ${GTK_LIBRARIES})
    add_subdirectory(lvgl-demos)
    add_compile_definitions(
        LV_USE_DEMO_MUSIC
        MUSIC_DEMO)

else()
    message("*******************************")
    message("*                             *")
    message("* Build setup for custom App  *")
    message("*                             *")
    message("*******************************")
    target_link_libraries(${NAME} 
        PRIVATE
        lvgl
        ${GTK_LIBRARIES})
endif()

include_directories( ${CMAKE_SOURCE_DIR})

add_subdirectory(lvgl-build)


###################### build gtk resources #####################
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
set(GRESOURCE_XML ${CMAKE_CURRENT_SOURCE_DIR}/resources/gresources.xml)
set(GLADE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/lvglsim.glade)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS
        --generate-source
        --sourcedir=resources
        --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
        ${GRESOURCE_XML}
    VERBATIM
    MAIN_DEPENDENCY ${GRESOURCE_XML}
    DEPENDS
        ${GLADE_FILE}
)

add_custom_target(
    gtk-resources
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)

add_dependencies(${NAME} gtk-resources)
