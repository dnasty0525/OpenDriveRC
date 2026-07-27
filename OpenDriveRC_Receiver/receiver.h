#pragma once

#include "protocol.h"

void receiverInit();

bool receiverUpdate();

const ControlPacket& getPacket();
