#include <Arduino.h>
#include <ClickButton.h>
#include <EEPROM.h>
// #include "RS485_protocol.h"
// #include "HabComProps.h"
#include "HabCom.h"

#define DbgMsg(value) (Serial.println(value))
#define DbgMsgInline(value) (Serial.print(value))
#define DbgMsgMode(value, mode) (Serial.println(value, mode))
// #define DbgMsg(value)
#define BTN_COUNT 4

// nastavit dle zapojení
#define RX_RO 11
#define TX_DI 12
#define TXE 10

bool changed = false;
bool HeartBeatReceived = false;

//TODO: nastavit tlačítka dle zapojení
ClickButton btn[BTN_COUNT] = {
  ClickButton(4, HIGH, false),
  ClickButton(5, HIGH, false),
  ClickButton(6, HIGH, false),
  ClickButton(7, HIGH, false)
};

unsigned long MsgSlotZeroTime;

//TODO: nastavit piny pro LED dle zapojení
byte ledPins[] = {8, 9};

byte myID;
byte data[HABCOM_DATA_LENGTH_BYTES];
HabCom::Message msg = {0,0,HabCom::CMD_NOT,0};

HabCom comm(RX_RO, TX_DI, HABCOM_BAUDRATE);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(TXE, OUTPUT);

  blink(8);

  comm.begin();
  Serial.begin(9600);

  DbgMsg("Init done.");

  myID = EEPROM.read(HABCOM_EEPROM_DEV_ID_ADDR);
  DbgMsg("ID storred in eeprom:");
  DbgMsg(myID);

  DbgMsg("Waiting for HEART_BEAT");

  while (!HeartBeatReceived) {
    if (!comm.available()) {
      delayMicroseconds(10);
      continue;
    }

    if (!comm.recvMsg(msg)) continue;

    MsgSlotZeroTime = micros();

    HeartBeatReceived = comm.isHeartBeat(msg);
    DbgMsgMode(msg.Cmd, BIN);
  }

  if (myID == 0x00 || myID == 0xFF) { //ID jeste nebylo nastaveno - ziskat z heart beatu
    myID = (msg.Data >> 4) & 0b11;
    EEPROM.write(HABCOM_EEPROM_DEV_ID_ADDR, myID);
    DbgMsg("New ID stored: " + myID);
  }

}

void loop() {
  for (int i=0; i<BTN_COUNT; i++) {

    btn[i].Update();
    // int func = btn[i].clicks;

  }
  delayMicroseconds(4);
}
