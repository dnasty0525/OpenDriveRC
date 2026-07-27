#include "steering.h"
#include "config.h"

#include <ESP32Servo.h>

Servo steeringServo;

void steeringInit()
{
    steeringServo.setPeriodHertz(50);

    steeringServo.attach(
        STEERING_PIN,
        1000,
        2000
    );

    steeringServo.writeMicroseconds(1500);
}

void steeringCenter()
{
    steeringServo.writeMicroseconds(1500);
}

void steeringUpdate(int steering)
{
    steering = constrain(steering, -1000, 1000);

    int pulse = map(
        steering,
        -1000,
        1000,
        1100,
        1900
    );

    steeringServo.writeMicroseconds(pulse);
}
