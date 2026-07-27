#include "receiver.h"

#include "protocol.h"
#include "config.h"

#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP udp;

ControlPacket packet;

const ControlPacket& getPacket()
{
    return packet;
}

void receiverInit()
{
    WiFi.mode(WIFI_STA);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    Serial.print("Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print(".");
    }

    Serial.println();

    Serial.println("Connected");

    Serial.println(WiFi.localIP());

    udp.begin(UDP_PORT);
}

    bool receiverUpdate()
{
    int packetSize = udp.parsePacket();

    if (packetSize > 0)
    {
        Serial.print("Received packet size: ");
        Serial.println(packetSize);
    }

    if (packetSize != sizeof(ControlPacket))
        return false;

    int len = udp.read(
        (uint8_t*)&packet,
        sizeof(ControlPacket)
    );

    if (len != sizeof(ControlPacket))
        return false;

    return true;
}
