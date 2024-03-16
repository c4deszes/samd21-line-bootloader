cmake_minimum_required(VERSION 3.20)

project(
    samd21-line-bootloader
    DESCRIPTION "Bootloader for SAMD21 devices over LINE protocol"
    LANGUAGES C CXX ASM
    VERSION 0.1.0)

option(SAMD21_LINE_BL_BUILD_TESTS "Enables library test cases" OFF)

find_package(Python REQUIRED)

include(tools/cmake/CPM.cmake)
CPMAddPackage("gh:c4deszes/samd21-hal#master")
CPMAddPackage("gh:c4deszes/bike-flash-tool#master")

# Documentation
#add_subdirectory(docs)

set(BL_GENCODE_DIR ${CMAKE_CURRENT_BINARY_DIR}/gencode)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/metainfo.h.in
               ${BL_GENCODE_DIR}/metainfo.h)

add_library(bootloader-api INTERFACE)
target_include_directories(bootloader-api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/api)
target_link_directories(bootloader-api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ld)

add_executable(
    bootloader
    EXCLUDE_FROM_ALL

    src/boot.c
    src/bootheader.c
    src/comm.c
    src/diag.c
    src/flash.c
    src/init.c
    src/main.c
    src/tasks.c
)
target_include_directories(bootloader PRIVATE include ${BL_GENCODE_DIR})
target_link_libraries(bootloader PRIVATE cmsis
                                         atsamd21e18a-dfp
                                         samd21-hal-api
                                         samd21-hal-sources
                                         samd21-hal-options
                                         line-protocol-api line-protocol-sources
                                         line-protocol-adapter-sources
                                         flash-line-api flash-line-sources
                                         bootloader-api
)
set_target_properties(bootloader PROPERTIES SUFFIX ".elf")
target_link_options(bootloader PRIVATE -Tbootloader.ld)
target_link_options(bootloader PRIVATE -Wl,-Map=bootloader.map -Wl,--gc-sections)

hexify(TARGET bootloader
       HEX ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex)
add_custom_target(bootloader-crc
    COMMAND srec_cat ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex --intel -crop 0x0000 0x1FFC
            -CRC32_Little_endian 0x1FFC --CCITT -o ${CMAKE_CURRENT_BINARY_DIR}/bootloader_with_crc.hex --intel
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bootloader.hex
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/bootloader_with_crc.hex
)

dump_symbols(bootloader ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lss)
dump_size(bootloader)

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

add_custom_target(bootloader-full
    DEPENDS bootloader-hex bootloader-crc bootloader-lss bootloader-memory
)

if(SAMD21_LINE_BL_BUILD_TESTS)
    add_subdirectory(tests/application)
endif()
