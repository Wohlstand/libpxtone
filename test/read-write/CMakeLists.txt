set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(read_write read_write.cpp)
target_link_libraries(read_write PRIVATE libpxtone_static)
add_dependencies(read_write libpxtone_static)

set_target_properties(read_write
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test-bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test-bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test-bin"
)

if(IS_BIG_ENDIAN)
    target_compile_definitions(read_write PRIVATE -Dpx_BIG_ENDIAN)
endif()

if(ENABLE_ADDRESS_SANITIZER)
    target_compile_options(read_write PRIVATE -fsanitize=address)
    target_link_options(read_write PRIVATE -fsanitize=address)
endif()

add_test(NAME ReadWriteTest
         COMMAND "${CMAKE_BINARY_DIR}/test-bin/read_write" "${FILES_SRC_DIR}" "${TEST_TEMP_DIR}")
