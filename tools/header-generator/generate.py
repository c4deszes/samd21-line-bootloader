# Python built-in modules
import argparse
import json

# Third-party modules
import intelhex

SERCOM0 = 0
SERCOM1 = 1
SERCOM2 = 2
SERCOM3 = 3

SERCOM_MUX_C = 2
SERCOM_MUX_D = 3

SERCOM_MAPPING = {
    "PA00_ALT": (SERCOM1, 0, SERCOM_MUX_D),
    "PA01_ALT": (SERCOM1, 1, SERCOM_MUX_D),

    "PA04_ALT": (SERCOM0, 0, SERCOM_MUX_D),
    "PA05_ALT": (SERCOM0, 1, SERCOM_MUX_D),
    "PA06_ALT": (SERCOM0, 2, SERCOM_MUX_D),
    "PA07_ALT": (SERCOM0, 3, SERCOM_MUX_D),

    "PA08": (SERCOM0, 0, SERCOM_MUX_C),
    "PA09": (SERCOM0, 1, SERCOM_MUX_C),
    "PA10": (SERCOM0, 2, SERCOM_MUX_C),
    "PA11": (SERCOM0, 3, SERCOM_MUX_C),

    "PA08_ALT": (SERCOM2, 0, SERCOM_MUX_D),
    "PA09_ALT": (SERCOM2, 1, SERCOM_MUX_D),
    "PA10_ALT": (SERCOM2, 2, SERCOM_MUX_D),
    "PA11_ALT": (SERCOM2, 3, SERCOM_MUX_D),

    "PA12": (SERCOM2, 0, SERCOM_MUX_C),
    "PA13": (SERCOM2, 1, SERCOM_MUX_C),
    "PA14": (SERCOM2, 2, SERCOM_MUX_C),
    "PA15": (SERCOM2, 3, SERCOM_MUX_C),
    # PA12, PA13, PA14, PA15 ALT only available on SAMD21G/J (SERCOM4)

    "PA16": (SERCOM1, 0, SERCOM_MUX_C),
    "PA17": (SERCOM1, 1, SERCOM_MUX_C),
    "PA18": (SERCOM1, 2, SERCOM_MUX_C),
    "PA19": (SERCOM1, 3, SERCOM_MUX_C),

    "PA16_ALT": (SERCOM3, 0, SERCOM_MUX_D),
    "PA17_ALT": (SERCOM3, 1, SERCOM_MUX_D),
    "PA18_ALT": (SERCOM3, 2, SERCOM_MUX_D),
    "PA19_ALT": (SERCOM3, 3, SERCOM_MUX_D),

    "PA22": (SERCOM3, 0, SERCOM_MUX_C),
    "PA23": (SERCOM3, 1, SERCOM_MUX_C),
    "PA24": (SERCOM3, 2, SERCOM_MUX_C),
    "PA25": (SERCOM3, 3, SERCOM_MUX_C)
    # PA22, PA23, PA24, PA25 ALT only available on SAMD21G/J (SERCOM5)
}

def crc32(msg):
    crc = 0xffffffff
    for b in msg:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xedb88320 if crc & 1 else crc >> 1
    return crc ^ 0xffffffff

def get_pin_settings(pin: str):
    if pin[1] == 'A':
        return (0, int(pin[2:4], base=10))
    raise ValueError('Invalid pin.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('config')
    parser.add_argument('--application')
    parser.add_argument('--output', required=True)
    args = parser.parse_args()

    with open(args.config) as f:
        config = json.load(f)

    output_file = intelhex.IntelHex()
    bootheader = []
    bootheader.append(1)    # V1
    bootheader += [0, 0, 0] # Padding

    if args.application:
        app_hex = intelhex.IntelHex(args.application)
        segments = app_hex.segments()
        # TODO: check if address space fits rows
        if len(segments) != 1:
            raise ValueError('Cant do CRC32 calculation on non continuous application.')
        crc = crc32(app_hex.tobinarray(segments[0][0], segments[0][1]-1))
        bootheader += list(int.to_bytes(segments[0][0], length=4, byteorder='little'))
        bootheader += list(int.to_bytes(segments[0][1], length=4, byteorder='little'))
        bootheader += list(int.to_bytes(crc, length=4, byteorder='little'))
    else:
        bootheader += list(int.to_bytes(0xFFFFFFFF, length=4, byteorder='little'))
        bootheader += list(int.to_bytes(0xFFFFFFFF, length=4, byteorder='little'))
        bootheader += list(int.to_bytes(0xFFFFFFFF, length=4, byteorder='little'))

    # Data section, unused for now
    bootheader += list(int.to_bytes(0xFFFFFFFF, length=4, byteorder='little'))
    bootheader += list(int.to_bytes(0xFFFFFFFF, length=4, byteorder='little'))
    bootheader += list(int.to_bytes(0xFFFFFFFF, length=4, byteorder='little'))

    tx_conf = SERCOM_MAPPING[config['txPin']]
    tx_pin = get_pin_settings(config['txPin'])
    rx_conf = SERCOM_MAPPING[config['rxPin']]
    rx_pin = get_pin_settings(config['rxPin'])

    if tx_conf[0] != rx_conf[0]:
        raise ValueError('TX and RX pins dont use the same SERCOM peripheral')
    
    if tx_conf[1] == rx_conf[1]:
        raise ValueError('TX and RX pads cant be the same.')

    if tx_conf[1] != 0 and tx_conf[1] != 2:
        raise ValueError('TX pad must be 0 or 2.')
    
    bootheader.append(tx_conf[0])      # SERCOM
    bootheader.append(int(config['baudrate'] / 4800)) # BAUDRATE
    bootheader.append(1 if config['oneWire'] else 0) # ONEWIRE

    bootheader.append(tx_pin[0])    # PORT
    bootheader.append(tx_pin[1])    # PIN
    bootheader.append(0 if tx_conf[1] == 0 else 1)   # PAD
    bootheader.append(tx_conf[2])   # MUX

    bootheader.append(rx_pin[0])    # PORT
    bootheader.append(rx_pin[1])    # PIN
    bootheader.append(rx_conf[1])   # PAD
    bootheader.append(rx_conf[2])   # MUX

    txe_pin = get_pin_settings(config['txePin']) if config['txePin'] else (0xFF, 0xFF)
    bootheader.append(txe_pin[0])
    bootheader.append(txe_pin[1])

    cs_pin = get_pin_settings(config['csPin']) if config['csPin'] else (0xFF, 0xFF)
    bootheader.append(cs_pin[0])
    bootheader.append(cs_pin[1])

    for i in range(len(bootheader), 252):
        bootheader += [0xFF]

    bootheader += int.to_bytes(crc32(bootheader), length=4, byteorder='little')

    output_file[0x3FF00:0x3FF00+256] = bootheader
    output_file.write_hex_file(args.output, byte_count=16)

    # 2. go through keys and add them to the data buffer
    # 3. validate each key
    # 4. if application is set then calculate CRC for it, otherwise skip


