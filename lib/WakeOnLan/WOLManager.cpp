#include "WOLManager.h"

WiFiUDP udp;


IPAddress broadcastIP()
{
    const IPAddress ip = WiFi.localIP();
    const IPAddress mask = WiFi.subnetMask();

    return IPAddress(
        ip[0] | (~mask[0] & 0xFF),
        ip[1] | (~mask[1] & 0xFF),
        ip[2] | (~mask[2] & 0xFF),
        ip[3] | (~mask[3] & 0xFF)
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
 
    // IPAddress IP(192, 168, 1, 255);
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

