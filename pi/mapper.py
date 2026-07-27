from config import *


class InputMapper:
    def deadzone(self, value):
        if abs(value) < STEERING_DEADZONE:
            return 0.0
        return value

    def expo(self, value):
        return (
            STEERING_EXPO * value ** 3
            + (1 - STEERING_EXPO) * value
        )

    def map_controls(self, steering, gas, brake):
        steering = self.deadzone(steering)
        steering = self.expo(steering)
        steering *= STEERING_LIMIT
        steering += (STEERING_TRIM / 1000.0)
        steering = max(-1.0, min(1.0, steering))
        steering_cmd = int(steering * 1000)

        gas = (1 - gas) / 2
        brake = (1 - brake) / 2
        throttle = gas - brake

        # G920 pedals often don't rest at exactly 0. Snap small offsets
        # back to true zero BEFORE scaling by THROTTLE_LIMIT, so a
        # nonzero resting value can't sneak through and block arming.
        if abs(throttle) < THROTTLE_DEADZONE:
            throttle = 0.0

        throttle *= THROTTLE_LIMIT
        throttle_cmd = int(throttle * 1000)

        return steering_cmd, throttle_cmd
