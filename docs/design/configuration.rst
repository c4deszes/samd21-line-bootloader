Configuration
=============

The bootloader is configurable through the bootheader, the bootheader is fixed as the last page of
the entire ROM area.

Application location
--------------------

The application can be placed anywhere in memory, these are indicated by the start and end addresses
in the bootheader.

The header's CRC and the application CRC stored in the header as well ensures that the addresses are
valid. The bootloader also checks whether the addresses are correct:

* Start and end on page boundaries
* Don't intersect the bootloader
* End address is higher than the start address
* Application interrupt vectors all point within the application boundaries

Serial communication settings
-----------------------------

The serial communication ports can be configured through the bootheader.

In case the bootheader is invalid or certain settings are invalid the settings or the whole port
configuration may revert to defaults.

.. list-table:: Communication settings
    :header-rows: 1

    * - Setting
      - Description
      - Invalid
      - Default

    * - Peripheral ID
      - SERCOMx to use
      - Discard full config
      - SERCOM3

    * - Baudrate
      - Baudrate in N*4800
      - Revert to default baudrate
      - 4 (19200)

    * - One wire mode
      - One wire mode
      - Revert to two-wire mode
      - Two wire mode

    * - TX port/pin/pad/mux
      - Serial transmit pin settings
      - Revert all pin settings to default
      - PA24, Pad2 (function C)

    * - RX port/pin/pad/mux
      - Serial receive pin settings
      - Revert all pin settings to default
      - PA25, Pad3 (function C)

    * - TXE port/pin
      - Transmit enable pin, drives this pin high when transmitting
      - Disable transmit enable feature
      - Disabled

    * - CS port/pin
      - Chip select pin, drives this pin high after boot
      - Disable chip select feature
      - Disabled
