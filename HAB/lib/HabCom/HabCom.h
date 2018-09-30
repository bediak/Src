/*
  HabCom.h - Knihovna pro komunikaci v rámci mého HomeAutomationBusu (HAB) po sběrnici RS485.
  Vytvořil Petr Přeučil, 19.červenec 2018.
  Všechna práva vyhrazena.
*/

#ifndef HabCom_h
#define HabCom_h

#include <Arduino.h>
#include <MySoftSerial.h>

#define HABCOM_MSG_LENGTH 16

// class MySoftSerial;

class HabCom
{
  public:
    enum CMD {
      CMD_NOT,
      HEART_BEAT,
      STATUS_INFO,
      CMD_GET,
      CMD_SET
    };

    enum DEV_STA {
      ERROR,
      BOOTING,
      READY,
      IDLE
    };

    enum RELAY_VALUE {
      RELAY_ON,
      RELAY_OFF,
      RELAY_SWITCH
    };

    enum BUTTON_STATE {
      BUTTON_IDLE,
      BUTTON_SHORT,
      BUTTON_DOUBLESHORT,
      BUTTON_TRIPLESHORT,
      BUTTON_LONG,
      BUTTON_DOUBLELONG,
      BUTTON_TRIPLELONG
    };

    enum LED_STATE {
      LED_OFF,
      LED_ON
    };

    static const byte BROADCAST = 0xFF;
    static const byte MASTER = 0x01;
    static const byte STX = 0x02;
    static const byte ETX = 0x03;

    HabCom(unsigned int speed = 9600);
    HabCom(int RX, int TX, unsigned int speed = 9600);
    void setSwPins(int RX, int TX);
    void setBaudrate(unsigned int speed);
    void setSendEnablePin(int pinNumber);
    void setStandByTime(unsigned long milliseconds);
    bool setDeviceAddr(const byte deviceAddr);
    bool begin();
    int available();
    void useSendEnablePin(bool value);
    byte recvMsgRaw(byte * data,                    // buffer to receive into
                  const byte length,              // maximum buffer size
                  unsigned long timeout = 250);          // milliseconds before timing out
    byte recvMsg(
                  byte & targetAddr,
                  CMD & command,
                  byte & sourceAddr,
                  byte * data,
                  unsigned long timeout = 250);
    bool sendMsg(
                  const byte ADDR,
                  const CMD command,
                  byte * data,
                  const byte length);
    void println(const int number);
    static void blink(byte count = 3, int duration = 200);

  private:
    bool _HwSerialUsed;
    int _speed, _TX, _RX;
    MySoftSerial _COM = MySoftSerial();
    bool _sendEnablePinSet = false;
    int _sendEnablePin;
    byte _deviceAddr = 0x00;
    unsigned long _standByTime = 100;
    unsigned long _lastMsgTime = 0;

    int read();
    byte write(byte what);
    static byte crc8 (const byte *addr, byte len);
    void sendComplemented (const byte what);

};

#endif
