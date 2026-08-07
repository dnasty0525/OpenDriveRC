# OpenDriveRC

A custom RC car control system that replaces a traditional radio transmitter/receiver with a **Logitech G920 racing wheel**, a **Raspberry Pi**, and an **ESP32**, communicating over Wi-Fi/UDP.

```
Logitech G920  --USB-->  Raspberry Pi  --WiFi/UDP-->  ESP32  --PWM-->  Steering Servo
   (wheel +                (Python)                 (Arduino)  --PWM-->  VXL-3s ESC -> Motor
    pedals)
```

The Pi reads the wheel and pedals, maps them into a compact binary packet, and streams it over UDP at 100Hz. The ESP32 receives packets, drives a steering servo directly, and drives the ESC through a dedicated arming/failsafe state machine so the motor can never move unexpectedly on startup or after a dropped connection.

## Hardware

- Logitech G920 wheel + pedals
- Raspberry Pi (4 or newer recommended), **MUST be running Raspberry Pi OS, use Raspberry Pi    Imager to install OS and connect to the same Wi-Fi network as the ESP32.
- Raspberry Pi Imager https://www.raspberrypi.com/software/
- Freenove ESP32-WROOM V1.3 (classic ESP32, **not** an S3/S2/C3 board)
- Traxxas Rustler (or similar) with a Traxxas VXL-3s brushless ESC
- Steering servo (stock chassis servo)
- 470–1000µF electrolytic capacitor
- Compatible battery pack for the ESC/motor

## Wiring

**ESP32 → Steering Servo**
```
Servo signal  -> GPIO18
Servo power   -> ESC BEC 5V
Servo ground  -> ESC ground
```

**ESP32 → VXL-3s ESC (3-wire receiver lead)**
```
Brown/Black (ground) -> ESP32 GND
Red (BEC +5V)         -> ESP32 5V / VIN
White (signal)         -> GPIO19
```

**Capacitor**

Place a 470–1000µF electrolytic capacitor across the 5V/GND rail right at the ESP32's power pins. The VXL-3s's BEC is sized for a couple of servos, not an ESP32 doing Wi-Fi — without this capacitor, transmit current spikes can brown out the ESP32 and cause intermittent signal loss.

**Power notes**

- During development, powering the ESP32's USB from a PC lets USB and the BEC share the 5V load, which can mask a marginal BEC.
- For real standalone operation, power should come from the BEC (with the capacitor above). Confirm the ESC/motor still behave correctly under BEC-only power before trusting it away from a bench.

## File Structure

### Raspberry Pi (`/pi`)

| File | Purpose |
|---|---|
| `main.py` | Main loop — reads the wheel, maps input, filters it, sends UDP packets at `SEND_RATE` |
| `controller.py` | `G920` class — reads wheel/pedal/button state via `pygame` |
| `mapper.py` | `InputMapper` — converts raw axes into steering/throttle commands (deadzone, expo, limits, trim) |
| `filters.py` | `LowPassFilter` — simple exponential smoothing used on both steering and throttle |
| `protocol.py` | `build_packet()` — packs commands into the binary `ControlPacket` format |
| `transmitter.py` | `UDPTransmitter` — sends the packed packet over UDP |
| `config.py` | All tunable Pi-side settings (network, deadzones, limits, smoothing) |

### ESP32 (`/OpenDriveRC_Receiver`)

| File | Purpose |
|---|---|
| `OpenDriveRC_Receiver.ino` | `setup()`/`loop()` — initializes everything, dispatches packets, drives the top-level failsafe |
| `config.h` | All tunable ESP32-side settings (Wi-Fi, pins, pulse widths, limits, failsafe timing) |
| `protocol.h` | `ControlPacket` struct — must stay byte-for-byte in sync with `protocol.py` |
| `receiver.h` / `receiver.cpp` | Wi-Fi/UDP setup and packet receiving |
| `steering.h` / `steering.cpp` | Steering servo control |
| `esc.h` / `esc.cpp` | ESC control — arming state machine, throttle smoothing, failsafe re-arm |

## Software Setup

### Raspberry Pi

1. Install Python 3 and `pygame`:
   ```bash
   pip install pygame
   ```
2. Connect the G920 wheel/pedals via USB.
3. Edit `config.py` — set `ESP32_IP` to match the IP address the ESP32 prints over serial on boot (see below), and confirm `UDP_PORT` matches `config.h` on the ESP32 (default `4210`).
4. Run:
   ```bash
   python main.py
   ```

### ESP32 (Arduino IDE)

1. Install ESP32 board support via Boards Manager if you haven't already.
2. **Board selection:** `Tools → Board → ESP32 → ESP32 Dev Module`. The Freenove WROOM V1.3 is a classic ESP32 with a USB-to-UART bridge chip, not a native-USB board — do not select an S2/S3/C3 profile.
3. Install the `ESP32Servo` library via Library Manager.
4. Edit `config.h` — set `WIFI_SSID` and `WIFI_PASSWORD` to match the same network the Pi is on.
5. Upload the sketch, then open the Serial Monitor at `115200` baud. On connect it will print the IP address it was assigned — copy that into `ESP32_IP` in the Pi's `config.py`.

## ESC Calibration (Traxxas VXL-3s / EZ-Set)

The VXL-3s must learn this system's specific pulse widths before it will respond correctly. This is a one-time step (redo it if you ever change `ESC_MIN_PULSE`/`ESC_MAX_PULSE`/`ESC_NEUTRAL_PULSE` in `config.h`).

1. **Wheels off the ground, or pinion gear removed.** Non-negotiable.
2. Temporarily set `THROTTLE_LIMIT = 1.0` in both `config.py` and `config.h`, reflash the ESP32, and restart `main.py`. The ESC needs to see true full-throttle/full-brake pulses (1000µs/2000µs) to calibrate correctly — the normal reduced training limit won't reach them.
3. With the Pi/ESP32 system running and feet fully off both pedals, wait a few seconds so the throttle has settled to true zero, then power on the ESC.
4. Press and hold the EZ-Set button — the LED turns green, then red. **Release the moment it blinks red once** — this captures neutral. Being patient about pedal position on this exact step matters most; a residual pedal offset at this instant becomes the ESC's reference "neutral" from then on.
5. Press the accelerator fully and hold until the LED confirms.
6. Release to neutral, then press the brake fully and hold until the LED confirms.
7. Release to neutral — calibration is complete.

If anything goes wrong mid-sequence, disconnect the battery, wait a few seconds, and restart from step 3.

Afterward, power cycle everything and set `THROTTLE_LIMIT` back down to a conservative value (e.g. `0.3`) for initial driving tests.

## First Power-On Checklist

Run through this in order, every time you change firmware:

1. Wheels off the ground / pinion removed.
2. Power on — confirm serial shows Wi-Fi connect, `ESC Ready`, then `ESC ARMED` once the pedals are neutral.
3. Confirm neutral produces no motor movement.
4. Apply light accelerator — confirm smooth forward spin.
5. Release — confirm it returns cleanly to neutral.
6. Apply brake — confirm expected response.
7. Disconnect the Pi/kill `main.py` — confirm the ESC returns to neutral within `FAILSAFE_TIME`.

Only after all of these pass should the truck go on the ground.

## Key Configuration Reference

**Pi (`config.py`)**

| Setting | Meaning |
|---|---|
| `ESP32_IP` / `UDP_PORT` | Must match the ESP32's assigned IP and `config.h`'s `UDP_PORT` |
| `SEND_RATE` | Packet send rate in Hz |
| `STEERING_DEADZONE` / `THROTTLE_DEADZONE` | Snaps small resting-axis offsets back to exact zero |
| `STEERING_EXPO` | Softens steering response near center |
| `STEERING_LIMIT` / `THROTTLE_LIMIT` | Scales output to a fraction of full range (this multiplies with the ESP32's own `THROTTLE_LIMIT`) |
| `STEERING_SMOOTHING` / `THROTTLE_SMOOTHING` | Low-pass filter strength on the Pi side |
| `STEERING_TRIM` | Steering center offset |

**ESP32 (`config.h`)**

| Setting | Meaning |
|---|---|
| `WIFI_SSID` / `WIFI_PASSWORD` | Network credentials |
| `STEERING_PIN` / `ESC_PIN` | GPIO18 / GPIO19 |
| `ESC_MIN_PULSE` / `ESC_NEUTRAL_PULSE` / `ESC_MAX_PULSE` | PWM pulse widths sent to the ESC |
| `THROTTLE_LIMIT` | ESP32-side throttle scaling (multiplies with the Pi's) |
| `THROTTLE_ALPHA` | Low-pass filter strength on the ESP32 side |
| `THROTTLE_DEADBAND` | Window (in raw ±1000 units) considered "neutral enough" to arm |
| `FAILSAFE_TIME` | Milliseconds without a packet before failsafe triggers |

## Troubleshooting Notes

- **Motor won't arm / never prints `ESC ARMED`:** check that `pkt.throttle` reads exactly `0` at rest on the ESP32 serial monitor. If not, the Pi-side throttle deadzone likely needs adjusting.
- **ESC beeps continuously at neutral only:** the calibrated neutral point doesn't match `ESC_NEUTRAL_PULSE`. Redo ESC calibration, being deliberate about letting the throttle fully settle to zero before releasing the EZ-Set button on the neutral step.
- **Motor keeps spinning after releasing the pedal, or ESC acts like signal is lost intermittently:** check for ESP32 brownout — confirm the capacitor is installed and consider a dedicated UBEC for the ESP32 instead of relying on the VXL-3s's BEC alone.
- **Direction feels wrong (accelerator/brake seem swapped or inverted):** this has consistently traced back to ESC calibration, not software — recalibrate before changing any code.

## Roadmap

- Vehicle profile system (`VehicleProfile` struct) so switching to a different RC car is a config change rather than a firmware rewrite.
- Extend `ControlPacket`/`VehicleCommand` for additional outputs (lights, horn, etc.) without breaking the existing communication layer.

