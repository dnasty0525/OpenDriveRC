import struct

PACKET_VERSION = 1

_sequence = 0


def build_packet(steering, throttle, buttons):

    global _sequence

    steering = int(max(-1000, min(1000, steering)))
    throttle = int(max(-1000, min(1000, throttle)))
    buttons = int(buttons)

    packet = struct.pack(
        "<BHhhHB",
        PACKET_VERSION,
        _sequence,
        steering,
        throttle,
        buttons,
        0
    )

    _sequence = (_sequence + 1) & 0xFFFF

    return packet
