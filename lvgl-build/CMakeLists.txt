set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../lvgl)

# Uncomment the followig line to build music demo
# add_compile_definitions(LV_USE_DEMO_MUSIC)

file(GLOB_RECURSE SOURCES 
	${LVGL_ROOT_DIR}/src/*.c
)

add_library(lvgl
	${SOURCES}
)

target_include_directories(lvgl PUBLIC
	.
	../lvgl/src
)
