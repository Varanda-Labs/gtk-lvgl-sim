set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../lvgl)

if(MUSIC_DEMO)
  add_compile_definitions(LV_USE_DEMO_MUSIC)
endif()

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
	${CMAKE_CURRENT_LIST_DIR}/../lvgl-build
	${CMAKE_CURRENT_LIST_DIR}/../lvgl/src
	${CMAKE_CURRENT_LIST_DIR}/../src/sim
	${CMAKE_CURRENT_LIST_DIR}/../
)
