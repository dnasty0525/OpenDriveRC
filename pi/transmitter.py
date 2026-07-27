import socket
from protocol import build_packet


class UDPTransmitter:

    def __init__(self, ip, port):

        self.address = (ip, port)

        self.sock = socket.socket(
            socket.AF_INET,
            socket.SOCK_DGRAM
        )

    def send(self, steering, throttle, buttons):

        packet = build_packet(
            steering,
            throttle,
            buttons
        )

        self.sock.sendto(
            packet,
            self.address
        )