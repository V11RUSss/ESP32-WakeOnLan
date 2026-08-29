#include "WOLManager.h"

WiFiUDP udp;


IPAddress broadcastIP()
{
    const IPAddress ip = WiFi.localIP();
    const IPAddress mask = WiFi.subnetMask();

    return IPAddress(
        ip[0] | static_cast<uint8_t>(~mask[0]),
        ip[1] | static_cast<uint8_t>(~mask[1]),
        ip[2] | static_cast<uint8_t>(~mask[2]),
        ip[3] | static_cast<uint8_t>(~mask[3])
    );
}


void createMagicPackage(uint8_t* package, const uint8_t* mac) 
{
    for (int i = 0; i < 6; i++) 
        package[i] = 0xFF;
    
    for (int i = 0; i < 16; i++) 
        for (int j = 0; j < 6; j++)
            package[i*6 + j + 6] = mac[j];
}


void sendWakeOnLan()
{
    uint8_t magicPackege[102];
 
    IPAddress IP = broadcastIP();

    createMagicPackage(magicPackege, MAC_PC);

    for (int i = 0; i < 3; i++) {
        if (udp.beginPacket(IP, PORT_WOL)) {
            udp.write(magicPackege, sizeof(magicPackege));
            udp.endPacket();
        }
        delay(100);
    }
}

