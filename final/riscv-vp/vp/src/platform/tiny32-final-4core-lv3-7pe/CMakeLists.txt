file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-tiny32-final-4core-lv3-7pe
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-tiny32-final-4core-lv3-7pe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tiny32-final-4core-lv3-7pe
        mc_main.cpp
        ${HEADERS})

target_link_libraries(tiny32-final-4core-lv3-7pe rv32 platform-tiny32-final-4core-lv3-7pe platform-common gdb-mc
	${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
