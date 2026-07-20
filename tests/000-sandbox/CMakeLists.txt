
get_filename_component(TARGET_NAME
		"${CMAKE_CURRENT_SOURCE_DIR}"
		NAME)

a3d_add_program(${TARGET_NAME}
		SOURCES
		App.cc
		App.h
		main.cc
		INSTALL_DESTINATION
		"${INSTALL_TESTS_EXEC_DIR}")



#
#set(UNIT_NUMBER "000")
#set(UNIT_DESCRIPTION "sandbox")
#set(TEST_NAME "${UNIT_NUMBER}-${UNIT_DESCRIPTION}")
#
#file(GLOB HEADERS *.h*)
#file(GLOB SOURCES *.c*)
#add_executable(${TEST_NAME} ${HEADERS} ${SOURCES})
#
#target_link_libraries(${TEST_NAME} avara3d)
#
#a3d_set_emscripten_compile_options(${TEST_NAME})
#a3d_set_emscripten_link_options(${TEST_NAME})
#
#install(TARGETS ${TEST_NAME}
#		DESTINATION "${INSTALL_TESTS_EXEC_DIR}")
