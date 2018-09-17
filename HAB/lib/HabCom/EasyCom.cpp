#include <Arduino.h>
#include "EasyCom.h"

EasyCom::EasyCom(unsigned int speed = 9600) {
  _HwSerialUsed = true;
  _speed = speed;
}

EasyCom::EasyCom(int TX, int RX, unsigned int speed = 9600) {
  _HwSerialUsed = false;
  _speed = speed;
  _TX = TX;
  _RX = RX;
}


void EasyCom::setSwPins(int TX, int RX) {
  _HwSerialUsed = false;
  _TX = TX;
  _RX = RX;
}

bool EasyCom::begin() {
  Serial.begin(_speed);
  return true;
}

int EasyCom::available() {
  return Serial.available();
}

int EasyCom::read() {
  int data = Serial.read();
  return data;
}

byte EasyCom::write(const byte what) {
  return Serial.write(what);
}

void EasyCom::blink(byte count = 3, int duration = 200) {
  for (byte i = 0; i<count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    digitalWrite(LED_BUILTIN, LOW);
    delay(duration/2);
  }
}

void EasyCom::println(const int number) {
  Serial.println(number);
}
