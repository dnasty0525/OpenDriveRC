#pragma once

#include <stdint.h>

#pragma pack(push,1)

struct ControlPacket
{
    uint8_t version;

    uint16_t sequence;

    int16_t steering;

    int16_t throttle;

    uint16_t buttons;

    uint8_t flags;
};

#pragma pack(pop)
