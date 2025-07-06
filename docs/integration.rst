Integrations
============

Concept
-------

Applications built on top of the bootloader must fulfill the following criterias:

* Used memory region must be appropriately offset from the bootloader
* Bootheader must be correctly set, including communication settings, addresses and CRCs
* RAM area must be appropriately placed, the application must be able to write the magic value to
  initiate bootloader entry

CMake
-----

The project can be included via CPM.CMake or as a submodule. The library brings a target that can
be used during integration, this target includes the necessary header file and an optional
linkerscript.

.. code-block:: cmake

    include(tools/cmake/CPM.cmake)
    CPMAddPackage("gh:c4deszes/samd21-line-bootloader@0.2.1")

    add_executable(application
      src/main.c
    )
    target_link_libraries(application PRIVATE bootloader-api)

Applications may choose to implement the key writing functionality themselves, to do that
simply define ``BL_BOOT_ENTRY_API_DISABLED`` macro.

An important aspect of this bootloader is the bootheader sector at the end of the memory, this
tells the bootloader where the application is going to be and what settings it should use for
serial communication. The library includes the `bootloader_header` function which will generate
a header from a configuration file, this header can then be merged into the application.

.. code-block:: cmake

    bootloader_header(
        TARGET app_header
        CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/tools/bootloader/boot_config.json
        APPLICATION ${CMAKE_CURRENT_BINARY_DIR}/application_noheader.hex
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/app_header.hex
    )

    add_custom_target(application-merge
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/app_header.hex ${CMAKE_CURRENT_BINARY_DIR}/application_noheader.hex
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/application.hex
    COMMAND srec_cat
            ${CMAKE_CURRENT_BINARY_DIR}/application_noheader.hex -intel
            ${CMAKE_CURRENT_BINARY_DIR}/app_header.hex -intel
            -o ${CMAKE_CURRENT_BINARY_DIR}/application.hex -intel
    )

To use the header generation a file such as the one below needs to be created. Using SERCOM-ALT pins
is possible by using `PA04_ALT` for example, the script will throw an error if the pin configuration
is not valid (e.g.: different SERCOMx used, wrong TX pin)

.. code-block:: json

    {
        "baudrate": 19200,
        "oneWire": true,
        "txPin": "PA24",
        "rxPin": "PA25",
        "csPin": "PA22",
        "txePin": null
    }
