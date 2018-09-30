/*
  HabCom.h - Knihovna pro komunikaci v rámci mého HomeAutomationBusu (HAB) po sběrnici RS485.
  Vytvořil Petr Přeučil, 19.červenec 2018.
  Všechna práva vyhrazena.
*/

#ifndef EasyCom_h
#define EasyCom_h

#include <Arduino.h>
#include <MySoftSerial.h>

class EasyCom
{
  public:
    EasyCom(unsigned int speed = 9600);
    EasyCom(int TX, int RX, unsigned int speed = 9600);
    void setBaudrate(unsigned int speed);
    void setSwPins(int TX, int RX);
    bool begin();
    int available();
    void println(const int number);
    int read();
    byte write(const byte what);
    void blink(byte count = 3, int duration = 200);

  private:
    MySoftSerial _COM = MySoftSerial();
    bool _HwSerialUsed;
    int _speed, _TX, _RX;
};

#endif
