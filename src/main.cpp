#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include "main.h"

//#define DEBUG

#ifdef DEBUG
const char *LIGHTTOPIC = "lumiereqqtest/set";
const char *PUMPTOPIC = "pompeqqtest/set";
const char *BANTOPIC = "bantest/set";
#else
const char *LIGHTTOPIC = "lumiereqq/set";
const char *PUMPTOPIC = "pompeqq/set";
const char *BANTOPIC = "ban/set";
#endif

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{

    Serial.begin(115200);

    delay(10);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        logging(".");
    }
    logging("WiFi connected\n");

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(BAN_PIN, OUTPUT);

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

void refreshrelay()
{
    digitalWrite(LIGHT_PIN, lightState);
    digitalWrite(PUMP_PIN, pumpState);

    logging("-refreshrelay-\n\tLight : ");
    logging(lightState, HEX);
    logging("\n\t Pumpe : ");
    logging(pumpState, HEX);
    logging("--\n");
}

void callback(char *topic, byte *payload, unsigned int length)
{
    logging("Message arrived [");
    logging(topic);
    logging("] ");
    if (!strcmp((char *)topic, BANTOPIC))
    {
        if (!strncmp((char *)payload, "OPEN", length))
            sendRemote(getoutban);
        else if (!strncmp((char *)payload, "CLOSE", length))
            sendRemote(enterban);
        else if (!strncmp((char *)payload, "STOP", length))
            sendRemote(stopban);
    }
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
    logging("\n");
    refreshrelay();
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        logging("Attempting MQTT connection...");
        // Attempt to connect
#ifdef DEBUG
        if (client.connect("ESP8266 Aquarium - Test"))
#else
        if (client.connect("ESP8266 Aquarium"))
#endif
        {
            logging("\nconnected\n");
            // ... and subscribe to topic
            client.subscribe(LIGHTTOPIC);
            client.subscribe(PUMPTOPIC);
            client.subscribe(BANTOPIC);
        }
        else
        {
            logging("failed, rc=");
            logging(client.state(), 10);
            logging(" try again in 5 seconds\n");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

// sendRemote : play button sequence
void sendRemote(uint8_t *bouton)
{
    start();
    sendSequence(bouton);
    start();
    sendSequence(bouton);
}

void sendSequence(uint8_t *bouton)
{
    for (uint8_t i = 0; i < 40; i++)
    {
        sendBit(bouton[i]);
    }
}

void sendBit(uint8_t b)
{
    if (b == 1)
    {
        digitalWrite(BAN_PIN, HIGH);
        delayMicroseconds(LONG_DELAY_US);
        digitalWrite(BAN_PIN, LOW);
        delayMicroseconds(SHORT_DELAY_US);
    }
    else
    {
        digitalWrite(BAN_PIN, HIGH);
        delayMicroseconds(SHORT_DELAY_US);
        digitalWrite(BAN_PIN, LOW);
        delayMicroseconds(LONG_DELAY_US);
    }
}

void start()
{
    digitalWrite(BAN_PIN, HIGH);
    delayMicroseconds(START_DELAY_US);
    digitalWrite(BAN_PIN, LOW);
    delayMicroseconds(START_DELAY_BEFORE_US);
}

void logging(const char *log_str)
{
#ifdef DEBUG
    Serial.print(log_str);
#endif
}

void logging(long n, int base)
{
#ifdef DEBUG
    Serial.print(n, base);
#endif
}
