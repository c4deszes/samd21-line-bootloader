import argparse
import intelhex
import json

SERCOM_MAPPING = {
            # SERCOM, PAD
    "PA08": (0, 0),
    "PA09": (0, 1),
    # TODO: add the rest
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
        return (0, int(pin[2:], base=10))
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
        if len(segments) != 1:
            raise ValueError('Cant do CRC32 calculation on non continuous application.')
        crc = crc32(app_hex[segments[0][0]:segments[0][1]])
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

    if tx_conf[1] != 0 and tx_conf[1] != 2:
        raise ValueError('TX pad must be 0 or 2.')
    
    bootheader.append(tx_conf[0])
    bootheader.append(int(config['baudrate'] / 4800))
    bootheader.append(1 if config['oneWire'] else 0)

    bootheader.append(tx_pin[0])    # PORT
    bootheader.append(tx_pin[1])    # PIN
    bootheader.append(tx_conf[1])   # PAD

    bootheader.append(rx_pin[0])    # PORT
    bootheader.append(rx_pin[1])    # PIN
    bootheader.append(rx_conf[1])   # PAD

    txe_pin = get_pin_settings(config['txePin']) if config['txePin'] else (0xFF, 0xFF)
    bootheader.append(txe_pin[0])
    bootheader.append(txe_pin[1])

    cs_pin = get_pin_settings(config['csPin']) if config['csPin'] else (0xFF, 0xFF)
    bootheader.append(cs_pin[0])
    bootheader.append(cs_pin[1])

    for i in range(len(bootheader), 124):
        bootheader += [0xFF]

    bootheader += int.to_bytes(crc32(bootheader), length=4, byteorder='little')

    output_file[0x3FF80:0x3FF80+128] = bootheader
    output_file.write_hex_file(args.output, byte_count=16)

    # 2. go through keys and add them to the data buffer
    # 3. validate each key
    # 4. if application is set then calculate CRC for it, otherwise skip


