#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>

const uint8_t DEFAULT_PUMP_STATE = HIGH;
const uint8_t DEFAULT_LIGHT_STATE = HIGH;

// Connect to the WiFi
const char *ssid = "*******";
const char *password = "**********";
const char *mqtt_server = "********";

const uint8_t PUMP_PIN = D0;
const uint8_t LIGHT_PIN = D3;

uint8_t lightState = DEFAULT_LIGHT_STATE;
uint8_t pumpState = DEFAULT_PUMP_STATE;