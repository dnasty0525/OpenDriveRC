#pragma once

// ============================
// WiFi
// ============================

#define WIFI_SSID      "7932CB-2.4"
#define WIFI_PASSWORD  "suzane1234"

#define UDP_PORT 4210

// ============================
// Servo
// ============================

#define STEERING_PIN 18

#define SERVO_LEFT   20
#define SERVO_CENTER 90
#define SERVO_RIGHT 160

#define STEERING_TRIM 0

// 0.0 = no smoothing
// 1.0 = extremely slow

#define STEERING_ALPHA 0.20f

// ============================
// Safety
// ============================

#define FAILSAFE_TIME 250

// ============================
// ESC
// ============================

#define ESC_PIN 19

// PWM pulse widths (microseconds)
#define ESC_MIN_PULSE      1000
#define ESC_NEUTRAL_PULSE  1500
#define ESC_MAX_PULSE      2000

// Initial throttle limit (30%)
#define THROTTLE_LIMIT 0.30f

// Throttle smoothing
#define THROTTLE_ALPHA 0.15f

// Neutral deadband for arming
#define THROTTLE_DEADBAND 50
