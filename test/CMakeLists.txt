set(TEST_TEMP_DIR "${CMAKE_BINARY_DIR}/test-temp")
set(FILES_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/files")

file(MAKE_DIRECTORY "${TEST_TEMP_DIR}")

add_subdirectory(read-write)

