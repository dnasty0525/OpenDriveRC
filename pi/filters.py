class LowPassFilter:
    def __init__(self, alpha):
        self.alpha = alpha
        self.value = 0.0

    def update(self, new_value):
        self.value = (
            self.value * (1.0 - self.alpha)
            + new_value * self.alpha
        )
        return self.value