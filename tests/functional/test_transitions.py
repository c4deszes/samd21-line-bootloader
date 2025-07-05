import time
import pytest

from line_protocol.protocol import LineTransportTimeout

class TestBootTransitions:

    # TODO: bootloader flashing could be moved to a fixture for reuse

    @pytest.mark.skip(reason="This test requires a specific setup with the LineMaster and serial port.")
    def test_ExitBoot_InvalidHeader(self, programmer, target, serial_number, line_master):
        # Setup
        target.mass_erase()
        programmer.program('bootloader_with_crc.hex')
        programmer.program('invalid_header.hex')

        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()

        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        op_status = line_master.get_operation_status(0xE)

        assert op_status == 'BootError'

    def test_ExitBoot_InvalidApp(self, programmer, target, serial_number, line_master, flash_tool):
        # Setup
        target.mass_erase()
        programmer.program('bootloader_with_crc.hex')
        programmer.program('valid_header.hex')

        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()

        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)
        op_status = line_master.get_operation_status(0xE)

        assert op_status == 'BootError'

        flash_tool.exit_bootloader(0xE, verify=False)
        time.sleep(1)

        # Confirms that the device was reset
        with pytest.raises(LineTransportTimeout):
            line_master.get_operation_status(0xE)

        line_master.conditional_change_address(sn, 0xE)
        op_status = line_master.get_operation_status(0xE)

        assert op_status == 'BootError'

    @pytest.mark.skip(reason="This test requires a specific setup with the LineMaster and serial port.")
    def test_EnterBoot_ValidApp(self, programmer, target, serial_number, line_master, flash_tool):
        # Setup
        target.mass_erase()
        programmer.program('bootloader_with_crc.hex')
        programmer.program('app_header.hex')
        programmer.program('application.hex')

        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(0.5)

        # Confirm that the application is running
        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'

        # Entering bootloader
        flash_tool.enter_bootloader(0xE, 0x1, sn)

        # Confirm that the bootloader is running
        line_master.conditional_change_address(sn, 0xE)
        op_status = line_master.get_operation_status(0xE)
        assert op_status == 'Boot'
