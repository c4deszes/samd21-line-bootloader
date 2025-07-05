import time
import pytest

from line_protocol.protocol import LineTransportTimeout
from line_flash.constants import *

class TestFlashing:

    @pytest.fixture(autouse=True, scope='function')
    def setup(self, programmer, target):
        # Setup
        target.mass_erase()
        programmer.program('bootloader_with_crc.hex')
        programmer.program('valid_header.hex')

    def test_Flashing_Bare(self, target, serial_number, line_master, flash_tool):
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        op_status = line_master.get_operation_status(0xE)
        assert op_status == 'BootError'

        flash_tool.flash_hex(0xE, 'application.hex')
        flash_tool.flash_hex(0xE, 'app_header.hex')

        # TODO: verify memory contents

        flash_tool.exit_bootloader(0xE, verify=False)
        time.sleep(1)

        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'

    def test_Flashing_ValidApp(self, programmer, target, serial_number, line_master, flash_tool):
        # Setup
        programmer.program('application.hex')
        programmer.program('app_header.hex')

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
        flash_tool.flash_hex(0xE, 'application.hex')
        flash_tool.flash_hex(0xE, 'app_header.hex')
        flash_tool.exit_bootloader(0xE, verify=False)
        time.sleep(1)

        # Confirming the new application is running
        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'
