/*
  HabCom.h - Knihovna pro komunikaci v rámci mého HomeAutomationBusu (HAB) po sběrnici RS485.
  Vytvořil Petr Přeučil, 19.červenec 2018.
  Všechna práva vyhrazena.
*/

#ifndef HabCom_h
#define HabCom_h

#include <Arduino.h>
#include <MySoftSerial.h>

#define HABCOM_VERSION 0.8
#define HABCOM_VERSION_DATE 20181002
#define HABCOM_MSG_LENGTH_BYTES 4
#define HABCOM_DATA_LENGTH_BYTES 2
#define HABCOM_MODE_LISTENER LOW
#define HABCOM_MODE_SENDER HIGH
#define HABCOM_MASTER_ADDR 0x00
#define HABCOM_TARGET_BROADCAST 0x3F // 6bit target addr: 11 11 11
#define HABCOM_EEPROM_DEV_ID_ADDR 0x00
#define HABCOM_BAUDRATE 57600
#define HABCOM_CYCLE_LENGTH 1024
#define HABCOM_MSG_START_TOLERANCE 2

void blink(byte count = 3, int duration = 200);

class HabCom
{
  public:
    enum Command {
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

    enum TERM {
          STX = '\2',   // start of text
          ETX = '\3'    // end of text
    };  // end of enum

    struct Message {
        public:
            byte Source;
            byte Target;
            HabCom::Command Cmd;
            word Data;
    };

    HabCom(const unsigned int speed = HABCOM_BAUDRATE);
    HabCom(const int RX, const int TX, const unsigned int speed = HABCOM_BAUDRATE);
    bool isHeartBeat(const Message &msg);
    void setSwPins(const int RX, const int TX);
    void setBaudrate(const unsigned int speed);
    void setSendEnablePin(const int pinNumber);
    void setStandByTime(const unsigned long milliseconds);
    bool setDeviceAddr(const byte deviceAddr);

    // allocate memory for buf_
    bool begin();
    void useSendEnablePin(const bool value);
    bool sendMsg(const HabCom::Message &msg);
    void println(const int number);

    ~HabCom() { stop(); }

    // free memory in buf_
    void stop ();

    // handle incoming data, return true if packet ready
    bool update ();

    // reset to no incoming data (eg. after a timeout)
    void reset ();

    bool available () const { return available_; };

    // once available, returns the address of the current message
    byte * getData ()   const { return data_; }
    void   getMsg (HabCom::Message &msg);
    byte   getLength () const { return inputPos_; }

    // return how many errors we have had
    unsigned long getErrorCount () const { return errorCount_; }

    // return when last packet started
    unsigned long getPacketStartTime () const { return startTime_; }

    // return true if a packet has started to be received
    bool isPacketStarted () const { return haveSTX_; }


  private:
    bool _HwSerialUsed;
    int _speed, _TX, _RX;
    MySoftSerial _COM = MySoftSerial();
    bool _sendEnablePinSet = false;
    int _sendEnablePin;
    byte _deviceAddr = 0x00;
    unsigned long _standByTime = 100;
    unsigned long _lastMsgTime = 0;
    byte * data_;

    bool available_;
    bool haveSTX_;
    unsigned long errorCount_;

    bool haveETX_;
    byte inputPos_;
    byte currentByte_;
    bool firstNibble_;
    unsigned long startTime_;

    int fAvailable();
    int fRead();
    size_t fWrite(const byte what);
    
    static byte crc8 (const byte *addr, byte len);
    void sendComplemented (const byte what);
    void MsgDataDecode( const byte *dataRAW, Message &msg);
    void MsgDataEncode(byte *dataRAW, const Message &msg);

};

#endif
