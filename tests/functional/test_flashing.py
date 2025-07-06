import time
import pytest
import logging

from line_protocol.protocol import LineTransportTimeout
from line_flash.constants import *

class TestFlashing:

    def test_Flashing_Bare(self, programmer, binaries, target, serial_number, line_master, flash_tool):
        """
        Tests the bootloader's ability to flash a new application when no application is present.
        The device should stay in bootloader mode and allow flashing of a new application.
        """
        target.mass_erase()
        programmer.program(binaries['bootloader'])
        programmer.program(binaries['factory_header'])
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        op_status = line_master.get_operation_status(0xE)
        assert op_status == 'BootError'

        flash_tool.flash_hex(0xE, binaries['application'])
        flash_tool.flash_hex(0xE, binaries['app_header'])

        flash_tool.exit_bootloader(0xE, verify=False)
        time.sleep(1)

        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'

        # TODO: verify memory contents

    def test_Flashing_ValidApp(self, binaries, programmer, target, serial_number, line_master, flash_tool):
        """
        Tests the bootloader's ability to flash a new application when a valid application is present.
        The device should first boot into the application, then be able to enter bootloader mode.
        After flashing the new application, it should be able to exit and run the new application.
        """
        # Setup
        target.mass_erase()
        programmer.program(binaries['bootloader'])
        programmer.program(binaries['application'])
        programmer.program(binaries['app_header'])

        sn = serial_number

        # Running the application
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'

        # Entering bootloader
        flash_tool.enter_bootloader(0xE, 0x1, sn)
        op_status = line_master.get_operation_status(0xE)
        assert op_status == 'Boot'

        # Flashing new application
        flash_tool.flash_hex(0xE, binaries['application'])
        flash_tool.flash_hex(0xE, binaries['app_header'])
        flash_tool.exit_bootloader(0xE, verify=False)
        time.sleep(1)

        # Confirming the new application is running
        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'
