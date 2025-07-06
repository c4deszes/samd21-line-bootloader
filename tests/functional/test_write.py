import time
import pytest

from line_protocol.protocol import LineTransportTimeout
from line_flash.constants import *
import random

class TestWrite:

    @pytest.fixture(autouse=True, scope='class')
    def setup(self, binaries, programmer, target):
        # Setup
        target.mass_erase()
        programmer.program(binaries['bootloader'])
        programmer.program(binaries['factory_header'])

    def test_WriteStatus_Initial(self, target, serial_number, line_master, flash_tool):
        """
        Tests whether the initial write status is set correctly when no pages have been written.
        The device should report that no pages have been written.
        """
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        write_status = flash_tool.get_write_status(0xE)

        assert write_status == FLASH_LINE_PAGE_NOT_WRITTEN

    def test_WriteStatus_OutOfBounds(self, target, serial_number, line_master, flash_tool):
        """
        Tests whether the write status is correctly reported when trying to write to an out-of-bounds address.
        The device should report an address error.
        """
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        # Write page with invalid address
        flash_tool.write_page(0xE, 0x0000, [0x00] * 64)
        time.sleep(0.1)
        write_status = flash_tool.get_write_status(0xE)

        assert write_status == FLASH_LINE_PAGE_ADDRESS_ERROR

    def test_WriteStatus_BadAlignment(self, target, serial_number, line_master, flash_tool):
        """
        Tests whether the write status is correctly reported when trying to write to an unaligned address.
        The device should report an address error.
        """
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        # Write page with bad alignment
        flash_tool.write_page(0xE, 0x4001, [0x00] * 64)
        time.sleep(0.1)
        write_status = flash_tool.get_write_status(0xE)

        assert write_status == FLASH_LINE_PAGE_ADDRESS_ERROR

    def test_WriteStatus_BadSize(self, target, serial_number, line_master, flash_tool):
        """
        Tests whether the write status is correctly reported when trying to write a page with an invalid size.
        The device should report a page size error.
        """
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        # Write page with invalid size
        flash_tool.write_page(0xE, 0x4000, [0x00] * 10)
        time.sleep(0.1)
        write_status = flash_tool.get_write_status(0xE)

        assert write_status == FLASH_LINE_PAGE_ADDRESS_ERROR

    def test_WriteStatus_ValidWrite(self, target, serial_number, line_master, flash_tool):
        """
        Tests whether the write status is correctly reported and the device writes the right page
        when requesting a valid write operation.
        The device should report success after writing a valid page.
        """
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        # Write a valid page
        content = [random.randint(0, 255) for _ in range(64)]

        flash_tool.write_page(0xE, 0x4000, content)
        time.sleep(0.1)
        write_status = flash_tool.get_write_status(0xE)

        assert write_status == FLASH_LINE_PAGE_WRITE_SUCCESS

        target.halt()

        # Verify the written page
        read_data = target.read_memory_block8(0x4000, 64)
        assert read_data == content
