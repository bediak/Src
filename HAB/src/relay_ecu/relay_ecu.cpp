#include <Arduino.h>
#include "HabCom.h"

#define DO_COUNT 16
#define DEV_ADDR 0xA1
#define TXE A5


int digouts[DO_COUNT] = {A4, A3, A2, A1, A0 , 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
unsigned long i = 0;

//char data[DATAGRAM_LENGTH];

byte data[HABCOM_MSG_LENGTH];
byte sourceAddr=0, targetAddr=0, data_length=0;
HabCom::CMD command = HabCom::CMD_NOT;

int pinNumber = 0;

HabCom COMM(9600);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // nastaveuje interne HabCom
  // pinMode(TXE, OUTPUT);
  // digitalWrite(TXE, LOW);

  // Serial.begin(9600);
  COMM.setDeviceAddr(DEV_ADDR);
  COMM.setSendEnablePin(TXE);
  COMM.useSendEnablePin(true);
  COMM.begin();
  // COMM.sendMsg(HabCom::BROADCAST, HabCom::STATUS_INFO, (byte)HabCom::BOOTING, 1);

  for (int i=0; i < DO_COUNT; i++)
  {
    pinMode(digouts[i], OUTPUT);
    digitalWrite(digouts[i], HabCom::RELAY_OFF);
  }
  // Serial.println("\nREADY!");
  //COMM.sendMsg(HabCom::BROADCAST, HabCom::STATUS_INFO, (byte)HabCom::READY, 1);
  HabCom::blink(15, 100);
}

void loop() {
  if (COMM.available() > 0) {
    data_length = COMM.recvMsg(targetAddr, command, sourceAddr, data);
    // Serial.println(command);
    if (data_length >= 0 && (targetAddr==DEV_ADDR || targetAddr == HabCom::BROADCAST)) {
      switch(command) {
        case HabCom::HEART_BEAT:
        case HabCom::STATUS_INFO:
          // COMM.sendMsg(sourceAddr, command, (byte)HabCom::IDLE, 1);
          break;
        case HabCom::CMD_SET:
          switch (data[1]) {
            case HabCom::RELAY_ON:
            case HabCom::RELAY_OFF:
              digitalWrite(digouts[data[0]], data[1]);
              break;
            case HabCom::RELAY_SWITCH:
              digitalWrite(digouts[data[0]], !digitalRead(digouts[data[0]]));
              break;
            default:
              break;
          }
        case HabCom::CMD_GET:
          byte answer[2];
          answer[0] = data[0];
          answer[1] = digitalRead(digouts[data[0]]);
          // COMM.sendMsg(sourceAddr, command, answer, 2);
          break;
        default: break;
      }
    }
  } else delay(10);
}
