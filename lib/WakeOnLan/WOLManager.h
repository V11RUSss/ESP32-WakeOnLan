#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>
#include <WiFi.h>

extern WiFiUDP udp;

const uint16_t PORT_WOL = 9;
const uint8_t MAC_PC[6] = {0xA8, 0xA1, 0x59, 0x71, 0x5C, 0x4E};

void sendWakeOnLan();
