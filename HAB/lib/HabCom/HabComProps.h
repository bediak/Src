/*
  HabComProps.h - Parametry protokolu pro komunikaci v rámci mého HomeAutomationBusu (HAB) po sběrnici RS485.
  Vytvořil Petr Přeučil, 16.září 2018.
  Všechna práva vyhrazena.
*/

#ifndef HabComProps_h
#define HabComProps_h

#include <Arduino.h>

#define HABCOM_MSG_LENGTH_BYTES 4
#define HABCOM_MODE_LISTENER LOW
#define HABCOM_MODE_SENDER HIGH
#define HABCOM_MASTER_ADDR 0x00
#define HABCOM_TARGET_BROADCAST 0x3F // 6bit target addr: 11 11 11
#define HABCOM_EEPROM_DEV_ID_ADDR 0x00

enum HabComCmd {
      CMD_NOT,
      HEART_BEAT,
      STATUS_INFO,
      CMD_GET,
      CMD_SET
    };

struct HabComMsg {
    public:
        byte Source;
        byte Target;
        HabComCmd Cmd;
        word Data;
};

#endif
