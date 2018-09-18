#include <Arduino.h>
#include <ClickButton.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "RS485_protocol.h"
#include "HabComProps.h"

#define DbgMsg(value) (Serial.println(value))
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
byte dataRaw[HABCOM_MSG_LENGTH_BYTES];
byte data_length=0;
HabComMsg msg = {0,0,CMD_NOT,0};

SoftwareSerial rs485(RX_RO,TX_DI);

inline void RS485_mode(int mode) {
  digitalWrite(TXE, mode);
}

inline void fWrite(const byte what) {
  RS485_mode(HABCOM_MODE_SENDER);
  rs485.write(what);
  delayMicroseconds(500);
  RS485_mode(HABCOM_MODE_LISTENER);
}

inline int fAvailable() {
  return rs485.available ();
}

inline int fRead() {
  return rs485.read ();
}

void blink(byte count = 3, int duration = 200) {
  for (byte i = 0; i<count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    digitalWrite(LED_BUILTIN, LOW);
    delay(duration/2);
  }
}

inline bool IsHeartBeat(HabComMsg &msg) {
  return (msg.Source == HABCOM_MASTER_ADDR && msg.Target == HABCOM_TARGET_BROADCAST && msg.Cmd == HEART_BEAT);
}

inline void MsgDataDecode(byte *dataRAW, HabComMsg &msg) {
  msg.Source = dataRaw[0] >> 2;
  msg.Target = (dataRaw[0] & 0b11) << 4 | dataRaw[1] >> 4;
  msg.Cmd    = (HabComCmd)(dataRaw[1] & 15);
  msg.Data   = (word)dataRaw[2] << 8 | (word)dataRaw[3];
}

inline void MsgDataEncode(byte *dataRAW, HabComMsg &msg) {
  dataRAW[0] = ((byte)msg.Source) << 2 | (((byte)msg.Target) >> 4) & 15;
  dataRAW[1] = ((byte)msg.Target) << 2 | (((byte)msg.Cmd) >> 4) & 15;
}

void MsgSend(word *data) {

  MsgDataEncode();
  

  sendMsg(fWrite, *data, HABCOM_MSG_LENGTH_BYTES );
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(TXE, OUTPUT);

  blink(8);

  RS485_mode(HABCOM_MODE_LISTENER);

  rs485.begin(57600);
  Serial.begin(9600);

  DbgMsg("Init done.");

  myID = EEPROM.read(HABCOM_EEPROM_DEV_ID_ADDR);
  DbgMsg("ID storred in eeprom:");
  DbgMsg(myID);

  DbgMsg("Waiting for HEART_BEAT");

  while (!HeartBeatReceived) {
    if (!rs485.available()) {
      delayMicroseconds(10);
      continue;
    }

    data_length = recvMsg(fAvailable, fRead, dataRaw, HABCOM_MSG_LENGTH_BYTES);
    if (data_length != 4) continue;

    MsgSlotZeroTime = micros();
    MsgDataDecode(dataRaw, msg);

    HeartBeatReceived = IsHeartBeat(msg);
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
