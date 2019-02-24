#include <SPI.h>
#include <Wire.h>
#include <secrets.h>

const uint8_t DEFAULT_PUMP_STATE = HIGH;
const uint8_t DEFAULT_LIGHT_STATE = HIGH;

const uint8_t PUMP_PIN = D2;
const uint8_t LIGHT_PIN = D3;
const uint8_t BAN_PIN = D4;

uint8_t lightState = DEFAULT_LIGHT_STATE;
uint8_t pumpState = DEFAULT_PUMP_STATE;

const uint16_t LONG_DELAY_US = 702;
const uint16_t SHORT_DELAY_US = 317;
const uint8_t REPEAT_DELAY_MS = 8;
const uint8_t REPEAT_NUM = 2;
const uint16_t START_DELAY_US = 4535;
const uint16_t START_DELAY_BEFORE_US = 1500;

//Static array from wave analysis
uint8_t enterban[] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
uint8_t getoutban[] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1};
uint8_t stopban[] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

void callback(char *topic, byte *payload, unsigned int length);
void sendBit(uint8_t b);
void sendSequence(uint8_t *bouton);
void sendRemote(uint8_t *bouton);
void refreshrelay();
void reconnect();
void start();
void logging (const char * log_str);
void logging (long n, int base);