import pytest
import configparser

from pyocd.core.helpers import ConnectHelper
from pyocd.core.target import Target
from pyocd.flash.file_programmer import FileProgrammer

from line_protocol.protocol import LineMaster, LineSerialTransport
from line_flash import FlashTool

@pytest.fixture(scope="session")
def config():
    config = configparser.ConfigParser()
    config.read(".config")
    yield config

@pytest.fixture(scope="session")
def binaries(config):
    """Fixture to provide the binary paths from the configuration."""
    yield {
        'bootloader': config['binaries']['bootloader'],
        'factory_header': config['binaries']['factory_header'],
        'app_header': config['binaries']['app_header'],
        'application': config['binaries']['application']
    }

@pytest.fixture(scope="session")
def env():
    """Fixture to provide the environment configuration."""
    config = configparser.ConfigParser()
    config.read(".env")
    yield config['env']

@pytest.fixture(scope="session")
def target_mcu(config):
    yield config['target']['mcu']

@pytest.fixture(scope="session")
def line_port(env):
    yield env['port']

@pytest.fixture(scope="class")
def session(target_mcu):
    with ConnectHelper.session_with_chosen_probe(blocking=False,
                                                 options={'target_override': target_mcu}) as session:
        yield session

@pytest.fixture(scope="class")
def target(session):
    """Fixture to provide the target object."""
    yield session.board.target

@pytest.fixture(scope="class")
def programmer(session):
    """Fixture to provide the FileProgrammer object."""
    yield FileProgrammer(session, keep_unwritten=True)

@pytest.fixture(scope="class")
def serial_number(target):
    """Fixture to read the serial number from the target."""
    d1 = target.read32(0x0080A00C)
    d2 = target.read32(0x0080A040)
    d3 = target.read32(0x0080A044)
    d4 = target.read32(0x0080A048)
    sn = d1 ^ d2 ^ d3 ^ d4
    yield sn

@pytest.fixture(scope="class")
def line_master(line_port):
    with LineSerialTransport(port=line_port) as transport:
        with LineMaster(transport) as master:
            yield master

@pytest.fixture(scope="class")
def flash_tool(line_master):
    yield FlashTool(line_master)
