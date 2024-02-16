add_executable(
    application
    EXCLUDE_FROM_ALL

    init.c
    main.c
    tasks.c
)
target_include_directories(application PRIVATE include ${BL_GENCODE_DIR})
target_link_libraries(application PRIVATE cmsis
                                         atsamd21e18a-dfp
                                         samd21-hal-api
                                         samd21-hal-sources
                                         samd21-hal-options
                                         line-protocol-api line-protocol-sources
                                         line-protocol-adapter-sources
                                         flash-line-api flash-line-sources
                                         bootloader-api
)
set_target_properties(application PROPERTIES SUFFIX ".elf")
target_link_directories(application PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_options(application PRIVATE -Tapplication.ld)
target_link_options(application PRIVATE -Wl,-Map=application.map -Wl,--gc-sections)

hexify(TARGET application
       HEX ${CMAKE_CURRENT_BINARY_DIR}/application.hex)

dump_symbols(application ${CMAKE_CURRENT_BINARY_DIR}/application.lss)
dump_size(application)

add_custom_target(application-full
    DEPENDS application-hex application-lss application-memory
)
