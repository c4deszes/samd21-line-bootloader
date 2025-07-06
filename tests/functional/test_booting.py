import time
import pytest

class TestBooting:

    @pytest.mark.skip(reason="Doesn't pass on normal targets, as the default UART port cannot be used")
    def test_Booting_Standalone(self, binaries, programmer, target, serial_number, line_master):
        """
        Tests the bootloader's ability to boot into a standalone application, in case the header
        is invalid or missing the device should stay in bootloader mode.
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

    def test_Booting_ValidHeader(self, binaries, programmer, target, serial_number, line_master):
        """
        Tests the bootloader's ability to boot with a valid bootheader but no application.
        The device should stay in bootloader mode and communicate with the right configuration.
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

    @pytest.mark.skip(reason="Not implemented yet")
    def test_Booting_AppBadCrc(self):
        """
        Tests the bootloader's ability to boot with a valid bootheader and a bad application CRC.
        The device should stay in bootloader mode and communicate with the right configuration.
        """
        pass

    @pytest.mark.skip(reason="Not implemented yet")
    def test_Booting_ValidApp(self):
        """
        Tests the bootloader's ability to boot with a valid bootheader and a valid application.
        The device should boot into the application and communicate with the application configuration.
        """
        pass
