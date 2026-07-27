#include "protocol.h"
#include "receiver.h"
#include "esc.h"
#include "steering.h"
#include "config.h"


// Last time we received a valid packet
unsigned long lastPacket = 0;


void setup()
{
    Serial.begin(115200);

    steeringInit();

    escInit();
    
    receiverInit();

    lastPacket = millis();

    Serial.print("ControlPacket size: ");
    Serial.println(sizeof(ControlPacket));
    Serial.println();
    Serial.println("===== OpenDriveRC Receiver =====");
}

void loop()
{
    if (receiverUpdate())
    {
        lastPacket = millis();

        const ControlPacket& pkt = getPacket();

    steeringUpdate(pkt.steering);
    escUpdate(pkt.throttle);

    Serial.print("SEQ: ");
    Serial.print(pkt.sequence);

    Serial.print("  Steering Raw: ");
    Serial.print(pkt.steering);


    Serial.print(" Throttle: ");
    Serial.println(pkt.throttle);
    }

    if (millis() - lastPacket > FAILSAFE_TIME)
{
    steeringCenter();
    escFailsafe();   // was escNeutral()
}
}
