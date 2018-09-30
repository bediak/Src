#include <Arduino.h>
#include "EasyCom.h"

EasyCom::EasyCom(unsigned int speed /* = 9600 */) :
  _HwSerialUsed(true),
  _speed(speed)
{
}

EasyCom::EasyCom(int TX, int RX, unsigned int speed /* = 9600 */) :
  _HwSerialUsed(false),
  _speed(speed),
  _TX(TX),
  _RX(RX)
{
  _COM.setPins(RX, TX);
}


void EasyCom::setBaudrate(unsigned int speed) {
  _speed = speed;
}

void EasyCom::setSwPins(int RX, int TX) {
  _HwSerialUsed = false;
  _TX = TX;
  _RX = RX;
  _COM.setPins(RX, TX);
}

bool EasyCom::begin() {
  if (_HwSerialUsed) {
    Serial.begin(_speed);
  }
  else {
    EasyCom::_COM.setPins(_RX, _TX);
    EasyCom::_COM.begin(_speed);
  }
  return true;
}

int EasyCom::available() {
  if (_HwSerialUsed)
    return Serial.available();
  else
    return EasyCom::_COM.available();

  return 0; // sem uz by se program nikdy nemel dostat, je to jen kvuli kompilatoru
}

int EasyCom::read() {
  int data = -1;
  if (_HwSerialUsed)
    data = Serial.read();
  else
    data = EasyCom::_COM.read();

  return data;
}

byte EasyCom::write(const byte what) {
  if (_HwSerialUsed)
    return Serial.write(what);
  else
    return EasyCom::_COM.write(what);
  
  return 0; // sem uz by se program nikdy nemel dostat, je to jen kvuli kompilatoru
}

void EasyCom::blink(byte count /* = 3 */, int duration /* = 200 */) {
  for (byte i = 0; i<count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    digitalWrite(LED_BUILTIN, LOW);
    delay(duration/2);
  }
}

void EasyCom::println(const int number) {
  if (_HwSerialUsed)
    Serial.println(number);
  else
    EasyCom::_COM.println(number);
}
