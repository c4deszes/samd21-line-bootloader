import time
import pytest

class TestBooting:

    @pytest.mark.skip(reason="Doesn't pass on normal targets, as the default UART port cannot be used")
    def test_Booting_Standalone(self, programmer, target, serial_number, line_master):
        # Setup
        target.mass_erase()
        programmer.program('bootloader_with_crc.hex')
        sn = serial_number

        # Running the bootloader
        target.reset_and_halt()
        target.resume()

        time.sleep(0.5)

        line_master.conditional_change_address(sn, 0xE)
        op_status = line_master.get_operation_status(0xE)

        assert op_status == 'BootError'

    def test_Booting_ValidHeader(self, programmer, target, serial_number, line_master):
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

    @pytest.mark.skip(reason="Not implemented yet")
    def test_Booting_AppBadCrc(self):
        pass

    @pytest.mark.skip(reason="Not implemented yet")
    def test_Booting_ValidApp(self):
        pass
