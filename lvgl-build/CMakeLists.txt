set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../lvgl)

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
