#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WOLManager.h>

#include "secrets.h"
#include "certificate.h"

WiFiClientSecure tlsClient;
PubSubClient mqttClient(tlsClient);

const char* TOPIC_WOL = "home/workstation/wake";
const char* TOPIC_STATUS = "home/workstation/status";
const char* CLIENT_ID = "esp32-home";

const uint32_t MQTT_RECONNECT_INTERVAL = 5000;
uint32_t lastMQTTReconnect = 0;

void connectWiFi() 
{
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(SSID_WIFI, PASS_WIFI);

    while (!WiFi.isConnected())
        delay(500);
}

bool connectMQTT() 
{
    bool result = mqttClient.connect(
        CLIENT_ID,
        MQTT_USER,
        MQTT_PASS,
        TOPIC_STATUS,
        1,
        true,
        "OFFLINE"
    );

    if (!result) {
        Serial.print("FAILED, state=");
        Serial.println(mqttClient.state());

        return false;
    }

    if (mqttClient.subscribe(TOPIC_WOL, 1))
        Serial.print("Subscribed: ");
    else
        Serial.println("Subscribe failed");


    mqttClient.publish(TOPIC_STATUS, "ONLINE", true);

    return true;
}

void mqttCallback(char* topic, byte* message, unsigned int length) 
{
    Serial.println();
    Serial.println("MQTT message received");

    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Payload: ");

    for (unsigned int i = 0; i < length; i++)
        Serial.print((char)message[i]);
    
    if (strcmp(topic, TOPIC_WOL) != 0)
        return;
    
    if (length == 6 && memcmp(message, "WAKEUP", 6) == 0) {
        sendWakeOnLan();

        mqttClient.publish(
            TOPIC_STATUS,
            "Wake up to a pure nation, clean air, bright future!", 
            true
        );
    }
    
}


void setup()
{
    Serial.begin(115200);

    delay(1000);

    connectWiFi();
    tlsClient.setCACert(ROOT_CA);

    mqttClient.setServer(MQTT_URL, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setKeepAlive(30);
    mqttClient.setSocketTimeout(5);

    connectMQTT();
}


void loop() 
{
    uint32_t now = millis();

    if (!WiFi.isConnected()) {
        if (mqttClient.connected())
            mqttClient.disconnect();

        delay(100);
        return;
    }

    if (!mqttClient.connected()) {
        if ((now - lastMQTTReconnect) >= MQTT_RECONNECT_INTERVAL) {
            lastMQTTReconnect = now;

            connectMQTT();
        }

        delay(10);
        return;
    }
    

    mqttClient.loop();
    delay(1);
}
