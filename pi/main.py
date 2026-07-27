import time

from controller import G920
from mapper import InputMapper
from transmitter import UDPTransmitter
from filters import LowPassFilter

from config import *

wheel = G920()

mapper = InputMapper()

radio = UDPTransmitter(
    ESP32_IP,
    UDP_PORT
)

steering_filter = LowPassFilter(
    STEERING_SMOOTHING
)

throttle_filter = LowPassFilter(
    THROTTLE_SMOOTHING
)

dt = 1.0 / SEND_RATE

print("OpenDriveRC Transmitter Started")

while True:

    steering, gas, brake, clutch, buttons = wheel.update()

    steering_cmd, throttle_cmd = mapper.map_controls(
        steering,
        gas,
        brake
    )

    steering_cmd = int(
        steering_filter.update(steering_cmd)
    )

    throttle_cmd = int(
        throttle_filter.update(throttle_cmd)
    )
    
    print(
        f"Steering: {steering_cmd:5d} ",
        f"Throttle: {throttle_cmd:5d} ",
        end="\r"
    )
    
    radio.send(
        steering_cmd,
        throttle_cmd,
        buttons
    )

    time.sleep(dt)
