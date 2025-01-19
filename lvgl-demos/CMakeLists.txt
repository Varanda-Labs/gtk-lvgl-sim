set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../lvgl)

# Uncomment the followig line to build music demo
add_compile_definitions(LV_USE_DEMO_MUSIC)

file(GLOB_RECURSE SOURCES 
	${LVGL_ROOT_DIR}/demos/*.c
)

add_library(demos
	${SOURCES}
)

target_link_libraries(demos 
    PRIVATE 
    lvgl
)

target_include_directories(demos PUBLIC
	.
	../lvgl-build
	../lvgl/src
)
