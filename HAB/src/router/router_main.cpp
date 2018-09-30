#include <Arduino.h>
#include "HabCom.h"

#define DbgMsg(value) (Serial.println(value))

#define RX_RO 11
#define TX_DI 12
#define TXE LED_BUILTIN

unsigned long MsgSlotZeroTime;
unsigned long time_temp;
unsigned long deltaTime = 0;

byte data[HABCOM_DATA_LENGTH_BYTES];
byte cDevices = 0;
HabCom::Message msg = {0,0,HabCom::CMD_NOT,0};
HabCom::Message msgHB = { HABCOM_MASTER_ADDR, HABCOM_TARGET_BROADCAST, HabCom::HEART_BEAT, 0 };

HabCom comm(RX_RO, TX_DI, HABCOM_BAUDRATE);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(TXE, OUTPUT);

  blink(8);

  comm.begin();
  Serial.begin(9600);

  DbgMsg("Router Init done.");

  MsgSlotZeroTime = millis() + 100;
}

void loop() {
    time_temp = millis();
    deltaTime = time_temp-MsgSlotZeroTime-HABCOM_CYCLE_LENGTH;
    if (deltaTime >= 0 && deltaTime <= HABCOM_MSG_START_TOLERANCE) {
        comm.sendMsg(msgHB);
        MsgSlotZeroTime = time_temp; 
        DbgMsg(deltaTime);
    }
    delayMicroseconds(4);
}
