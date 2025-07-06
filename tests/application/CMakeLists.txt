samx21_hal(hal-test-library ATSAMD21E18A)

add_executable(
    application
    EXCLUDE_FROM_ALL

    init.c
    main.c
    tasks.c
)
target_include_directories(application PRIVATE include ${BL_GENCODE_DIR})
target_link_libraries(application PRIVATE hal-test-library
                                         line-protocol-api line-protocol-sources
                                         line-protocol-adapter-sources
                                         flash-line-api flash-line-sources
                                         bootloader-api
)
set_target_properties(application PROPERTIES SUFFIX ".elf")
target_link_directories(application PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_options(application PRIVATE -Tapplication.ld)
target_link_options(application PRIVATE -Wl,-Map=application.map -Wl,--gc-sections)

set(SAMD21_LINE_BL_APP "${CMAKE_CURRENT_BINARY_DIR}/application.hex" CACHE INTERNAL "")
set(SAMD21_LINE_BL_APP_HEADER "${CMAKE_CURRENT_BINARY_DIR}/app_header.hex" CACHE INTERNAL "")
set(SAMD21_LINE_BL_FACTORY_HEADER "${CMAKE_CURRENT_BINARY_DIR}/factory_header.hex" CACHE INTERNAL "")

hexify(TARGET application
       HEX ${SAMD21_LINE_BL_APP})

bootloader_header(
    TARGET factory_header
    CONFIG config.json
    OUTPUT ${SAMD21_LINE_BL_FACTORY_HEADER}
)

bootloader_header(
    TARGET app_header
    CONFIG config.json
    APPLICATION ${SAMD21_LINE_BL_APP}
    OUTPUT ${SAMD21_LINE_BL_APP_HEADER}
)

dump_symbols(application ${CMAKE_CURRENT_BINARY_DIR}/application.lss)
dump_size(application)

add_custom_target(application-full
    DEPENDS application-hex application-lss application-memory
)
