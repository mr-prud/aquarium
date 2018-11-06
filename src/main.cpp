#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "font.h"
#include "main.h"

//#define DEBUG

SSD1306Wire display(0x3c, D2, D1);

#ifdef DEBUG
const char *LIGHTTOPIC = "lumiereqqtest/set";
const char *PUMPTOPIC = "pompeqqtest/set";
#else
const char *LIGHTTOPIC = "lumiereqq/set";
const char *PUMPTOPIC = "pompeqq/set";
#endif

WiFiClient espClient;
PubSubClient client(espClient);

void displayState();

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
    displayState();
}
void printState(int x, int y, int state)
{
    if (state == HIGH)
        display.drawString(x, y, "ON");
    else
        display.drawString(x, y, "OFF");
}
void displayState()
{
    display.clear();
    display.drawString(0, 0, "LIGHT :");
    printState(50, 0, lightState);
    display.drawString(0, 10, "PUMP  :");
    printState(50, 10, pumpState);
    client.connected();
    display.drawString(0, 20, "MQTT  : ");
    if (client.connected())
    {
        display.drawString(50, 20, "YES");
    }
    else
    {
        display.drawString(50, 20, "NO");
    }
    display.display();
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
        if (client.connect("ESP8266 Client 1.1.0-beta"))
        {
            Serial.println("connected");
            client.subscribe(LIGHTTOPIC);
            client.subscribe(PUMPTOPIC);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");

            delay(5000);
        }
    }
    refreshrelay();
}

void setup()
{

    Serial.begin(115200);

    delay(10);

    display.init();
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Roboto_Mono_Light_10);
    display.drawString(0, 0, "Startup");
    display.display();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        display.print(".");
        display.display();
    }
    display.drawString(0, 10, "WiFi connected");
    display.display();

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
