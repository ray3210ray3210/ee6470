file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-tiny32-final-2core-lv2-3pe
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-tiny32-final-2core-lv2-3pe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tiny32-final-2core-lv2-3pe
        mc_main.cpp
        ${HEADERS})

target_link_libraries(tiny32-final-2core-lv2-3pe rv32 platform-tiny32-final-2core-lv2-3pe platform-common gdb-mc
	${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
