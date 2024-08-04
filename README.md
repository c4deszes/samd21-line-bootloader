# SAMD21 LINE Bootloader

Bootloader for SAMD21 devices using [LINE protocol]() with the flash service addon.

## Open issues

- Support for shutdown and idle diagnostic callouts
- Automatic boot exit if no command is received for a few seconds
- Update to latest LINE protocol library
- Enable watchdog
- Testing boot protection
- Flexible page write option

## Deferred tasks

- Support for all SERCOM configurations and pinouts
- Support for other microcontrollers (ATSAMC21 and smaller sizes)
