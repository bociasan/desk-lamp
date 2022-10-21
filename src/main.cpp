#include <Arduino.h>
#include <StripBS.h>

// #include <EncoderBS.h>

// #define pinA D6
// #define pinB D7
// #define pinSw D8

#define pinStrip D1
#define useSerial true

#define k 0.02
#define brightness 90
#define changingDelay 15

// EncoderBS enc(pinA, pinB, pinSw, useSerial);
StripBS ledStrip(pinStrip, brightness, changingDelay, k, useSerial);


void setup() {
  Serial.begin(115200);
}

void loop() {
  // enc.tick();
  ledStrip.tick();
}