#include "esc.h"
#include "config.h"

#include <ESP32Servo.h>
#include <Arduino.h>

Servo esc;

enum ESCState
{
    BOOT,
    WAIT_FOR_NEUTRAL,
    ARMED
};

static ESCState state = BOOT;
static float filteredThrottle = 0;

void escInit()
{
    esc.setPeriodHertz(50);
    esc.attach(ESC_PIN, ESC_MIN_PULSE, ESC_MAX_PULSE);
    esc.writeMicroseconds(ESC_NEUTRAL_PULSE);

    delay(3000);

    filteredThrottle = 0;
    state = WAIT_FOR_NEUTRAL;

    Serial.println("ESC Ready");
}

void escNeutral()
{
    esc.writeMicroseconds(ESC_NEUTRAL_PULSE);
}

void escFailsafe()
{
    escNeutral();
    filteredThrottle = 0;

    if (state == ARMED)
    {
        Serial.println("ESC FAILSAFE - re-arm required");
    }

    state = WAIT_FOR_NEUTRAL;
}

bool escArmed()
{
    return state == ARMED;
}

void escUpdate(int throttle)
{
    filteredThrottle =
        filteredThrottle * (1.0f - THROTTLE_ALPHA)
        + throttle * THROTTLE_ALPHA;

    switch (state)
    {
        case WAIT_FOR_NEUTRAL:
            // Write neutral WITHOUT resetting filteredThrottle here -
            // that was the original bug. We need to test the real
            // filtered pedal value, not a value we just zeroed.
            esc.writeMicroseconds(ESC_NEUTRAL_PULSE);

            if (abs(filteredThrottle) < THROTTLE_DEADBAND)
            {
                state = ARMED;
                Serial.println("ESC ARMED");
            }
            return;

        case ARMED:
            break;

        default:
            return;
    }

    float limited = filteredThrottle * THROTTLE_LIMIT;

    int pulse = map(
        (int)limited,
        -1000, 1000,
        ESC_MIN_PULSE, ESC_MAX_PULSE
    );

    pulse = constrain(pulse, ESC_MIN_PULSE, ESC_MAX_PULSE);

    esc.writeMicroseconds(pulse);
}
