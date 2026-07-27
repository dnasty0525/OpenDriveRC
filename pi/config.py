# ============================================
# OpenDriveRC Configuration
# ============================================

# Network
ESP32_IP = "192.168.4.37"
UDP_PORT = 4210

# Send rate
SEND_RATE = 100  # Hz

# Steering
STEERING_DEADZONE = 0.03
STEERING_EXPO = 0.35
STEERING_SMOOTHING = 0.18
STEERING_TRIM = 0          # -1000..1000 scale adjustment
STEERING_LIMIT = 1.00      # 1.0 = 100%

# Servo travel (for future dashboard)
STEERING_LEFT = 20
STEERING_RIGHT = 160

# Throttle
THROTTLE_DEADZONE = 0.03
THROTTLE_LIMIT = 0.40
THROTTLE_SMOOTHING = 0.15

# Safety
FAILSAFE_TIMEOUT = 0.25
PROTOCOL_VERSION = 1