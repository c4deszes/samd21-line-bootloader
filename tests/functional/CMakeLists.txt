
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.config.in
               ${CMAKE_CURRENT_SOURCE_DIR}/.config
               @ONLY)

pytest_discover_tests(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    TEST_PREFIX "FUNC_"
    XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/report
)
