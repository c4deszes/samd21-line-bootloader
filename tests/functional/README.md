# Test plan

## Booting

1. When BL is flashed by itself
    - Assert that header is detected invalid
    - Assert that device is in boot error op state

2. When BL is flashed with a valid header (No app)
    - Assert header is valid
    - Assert app invalid
    - Assert boot_error

3. When BL is flashed with a valid header (invalid app)
    - Assert header is valid
    - Assert app invalid
    - Assert boot_error

4. When BL is flashed along a valid app and header
    - Assert op mode ok

## Boot transitions

1. When BL ok, header invalid
    Request exit boot
    Assert reentry to boot_error

2. When BL ok, header and app valid
    Assert op mode ok
    Request boot entry

## Flashing

1. When BL is ok, app invalid
