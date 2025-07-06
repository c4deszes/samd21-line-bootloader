cmake_minimum_required(VERSION 3.20)

project(
    samd21-line-bootloader
    DESCRIPTION "Bootloader for SAMD21 devices over LINE protocol"
    LANGUAGES C CXX ASM
    VERSION 0.4.0)

################################################################################
# Options
################################################################################
option(SAMD21_LINE_BL_BUILD_BINARY "Enables building the bootloader" OFF)
option(SAMD21_LINE_BL_BUILD_TESTS "Enables library test cases" OFF)
option(SAMD21_LINE_BL_BUILD_FUNCTIONAL_TESTS "Running functional test cases" OFF)
option(SAMD21_LINE_BL_BUILD_DOCS "Enables documentation generation" OFF)

find_package(Python REQUIRED)

if(SAMD21_LINE_BL_BUILD_TESTS)
enable_testing()
include(${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/Pytest.cmake)
endif()

################################################################################
# Dependencies
################################################################################
include(tools/cmake/CPM.cmake)
CPMAddPackage("gh:c4deszes/bike-line-protocol@0.3.0")
CPMAddPackage("gh:c4deszes/samd21-hal@0.4.0")
CPMAddPackage("gh:c4deszes/bike-flash-tool@0.2.0")

################################################################################
# Interface
################################################################################
add_library(bootloader-api INTERFACE)
target_sources(bootloader-api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/key.c)
target_include_directories(bootloader-api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/api)
target_link_directories(bootloader-api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ld)

################################################################################
# Sources
################################################################################
if (SAMD21_LINE_BL_BUILD_BINARY)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(BL_GENCODE_DIR ${CMAKE_CURRENT_BINARY_DIR}/gencode)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/metainfo.h.in
               ${BL_GENCODE_DIR}/metainfo.h)

set(SAMD21_LINE_BL_MCU ATSAMD21E18A)
samx21_hal(hal-library ${SAMD21_LINE_BL_MCU})

add_executable(
    bootloader
    EXCLUDE_FROM_ALL

    src/boot.c
    src/bootheader.c
    src/comm.c
    src/flash.c
    src/init.c
    src/main.c
    src/tasks.c
)
target_include_directories(bootloader PRIVATE include ${BL_GENCODE_DIR})
target_link_libraries(bootloader PRIVATE hal-library
                                         line-protocol-api line-protocol-sources
                                         line-protocol-adapter-sources
                                         flash-line-api flash-line-sources
                                         bootloader-api
)
set_target_properties(bootloader PROPERTIES SUFFIX ".elf")
set_target_properties(bootloader PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ld/bootloader.ld;${CMAKE_CURRENT_SOURCE_DIR}/ld/memory.ld")
target_link_options(bootloader PRIVATE -Tbootloader.ld)
target_link_options(bootloader PRIVATE -Wl,-Map=bootloader.map -Wl,--gc-sections)

hexify(
    TARGET bootloader
    HEX ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex)
dump_symbols(bootloader ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lss)
dump_size(bootloader)

set(SAMD21_LINE_BL_BINARY ${CMAKE_CURRENT_BINARY_DIR}/bootloader_with_crc.hex)

add_custom_target(bootloader-crc
    COMMAND srec_cat ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex --intel -crop 0x0000 0x2FFC
            -CRC32_Little_endian 0x2FFC --CCITT -o ${CMAKE_CURRENT_BINARY_DIR}/bootloader_with_crc.hex --intel
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/bootloader_with_crc.hex
)

add_custom_target(bootloader-full
    DEPENDS bootloader-hex bootloader-crc bootloader-lss bootloader-memory
)

endif()

################################################################################
# Functions
################################################################################
function(bootloader_header)
cmake_parse_arguments(
    ""
    ""
    "TARGET;OUTPUT;CONFIG;APPLICATION"
    ""
    ${ARGN}
)

    if(DEFINED _APPLICATION)
        add_custom_target(
            ${_TARGET}
            COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tools/header-generator/generate.py
                    ${_CONFIG} --application ${_APPLICATION} --output ${_OUTPUT}
            DEPENDS ${_CONFIG} ${_APPLICATION}
            BYPRODUCTS ${_OUTPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    else()
        add_custom_target(
            ${_TARGET}
            COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tools/header-generator/generate.py
                    ${_CONFIG} --output ${_OUTPUT}
            DEPENDS ${_CONFIG}
            BYPRODUCTS ${_OUTPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endfunction()

################################################################################
# Testing
################################################################################
if(SAMD21_LINE_BL_BUILD_TESTS)
    add_subdirectory(tests/application)
endif()
if(SAMD21_LINE_BL_BUILD_FUNCTIONAL_TESTS)
    add_subdirectory(tests/functional)
endif()

################################################################################
# Documentation
################################################################################
if (SAMD21_LINE_BL_BUILD_DOCS)
    add_subdirectory(docs)
endif()
