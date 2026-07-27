import pygame


class G920:

    def __init__(self):

        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            raise Exception("No G920 Connected")

        self.js = pygame.joystick.Joystick(0)
        self.js.init()

        print("Connected:", self.js.get_name())

    def update(self):

        pygame.event.pump()

        steering = self.js.get_axis(0)
        gas = self.js.get_axis(1)
        brake = self.js.get_axis(2)
        clutch = self.js.get_axis(3)

        # Convert button states into a 16-bit bitmask
        button_mask = 0

        num_buttons = min(self.js.get_numbuttons(), 16)

        for i in range(num_buttons):
            if self.js.get_button(i):
                button_mask |= (1 << i)

        return steering, gas, brake, clutch, button_mask
