import time
import pytest
import logging

from line_protocol.protocol import LineTransportTimeout

class TestBootTransitions:

    # TODO: bootloader flashing could be moved to a fixture for reuse

    @pytest.mark.skip(reason="This test requires a specific setup with the LineMaster and serial port.")
    def test_ExitBoot_InvalidHeader(self, binaries, programmer, target, serial_number, line_master):
        """
        Tests the bootloader's ability to handle an invalid application header.
        The device should stay in bootloader mode and not transition to the application, when boot
        exit is requested it should reset into the bootloader.
        """
        # Setup
        target.mass_erase()
        programmer.program(binaries['bootloader'])

        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()

        time.sleep(0.5)
        line_master.conditional_change_address(sn, 0xE)

        op_status = line_master.get_operation_status(0xE)

        assert op_status == 'BootError'

    def test_ExitBoot_InvalidApp(self, binaries, programmer, target, serial_number, line_master, flash_tool):
        """
        Tests the bootloader's ability to handle an invalid application.
        The device should stay in bootloader mode and not transition to the application, when boot
        exit is requested it should reset into the bootloader.
        """
        # Setup
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

        flash_tool.exit_bootloader(0xE, verify=False)
        time.sleep(1)

        # Confirms that the device was reset
        with pytest.raises(LineTransportTimeout):
            line_master.get_operation_status(0xE)

        line_master.conditional_change_address(sn, 0xE)
        op_status = line_master.get_operation_status(0xE)

        assert op_status == 'BootError'

    # Way the application is flashed hardfaults the microcontroller
    def test_EnterBoot_ValidApp(self, binaries, programmer, target, serial_number, line_master, flash_tool):
        """
        Tests the bootloader's ability to enter bootloader mode from a valid application.
        The device should first boot into the application, then be able to enter bootloader mode.
        After entering bootloader mode, it should be able to flash a new application.
        """
        # Setup
        target.mass_erase()
        programmer.program(binaries['bootloader'])
        programmer.program(binaries['app_header'])
        programmer.program(binaries['application'])

        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()
        time.sleep(2)

        #target.reset_and_halt()
        #target.resume()
        #time.sleep(1)
        target.halt()
        logging.getLogger().info("pc: 0x%X", target.read_core_register("pc"))
        target.resume()

        # Confirm that the application is running
        op_status = line_master.get_operation_status(0x1)
        assert op_status == 'Ok'

        # Entering bootloader
        flash_tool.enter_bootloader(0xE, 0x1, sn)

        # Confirm that the bootloader is running
        line_master.conditional_change_address(sn, 0xE)
        op_status = line_master.get_operation_status(0xE)
        assert op_status == 'Boot'
