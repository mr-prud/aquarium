#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include "main.h"

//#define DEBUG

#ifdef DEBUG
const char *LIGHTTOPIC = "lumiereqqtest/set";
const char *PUMPTOPIC = "pompeqqtest/set";
#else
const char *LIGHTTOPIC = "lumiereqq/set";
const char *PUMPTOPIC = "pompeqq/set";
#endif

WiFiClient espClient;
PubSubClient client(espClient);

void refreshrelay()
{
    digitalWrite(LIGHT_PIN, lightState);
    digitalWrite(PUMP_PIN, pumpState);
#ifdef DEBUG
    Serial.println("-refreshrelay-");
    Serial.print("\tLight : ");
    Serial.print(lightState, HEX);
    Serial.print(" Pumpe : ");
    Serial.println(pumpState, HEX);
    Serial.println("--");
#endif
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    if (!strcmp((char *)topic, LIGHTTOPIC))
    {
        if (!strncmp((char *)payload, "ON", length))
            lightState = HIGH;
        else if (!strncmp((char *)payload, "OFF", length))
            lightState = LOW;
    }
    else if (!strcmp((char *)topic, PUMPTOPIC))
    {
        if (!strncmp((char *)payload, "ON", length))
            pumpState = HIGH;
        else if (!strncmp((char *)payload, "OFF", length))
            pumpState = LOW;
    }
    Serial.println();
    refreshrelay();
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266 Client test"))
        {
            Serial.println("connected");
            // ... and subscribe to topic
            client.subscribe(LIGHTTOPIC);
            client.subscribe(PUMPTOPIC);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{

    Serial.begin(115200);

    delay(10);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.print("WiFi connected");

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);

    refreshrelay();
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();
    yield();
}
