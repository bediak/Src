#include <Arduino.h>
#include "HabCom.h"

#define DbgMsg(value) (Serial.println(value))
#define DbgMsgInline(value) (Serial.print(value))

#define RX_RO 11
#define TX_DI 12
#define TXE LED_BUILTIN

unsigned long CurrentTime = 0;
unsigned long NextSlotTime = 0;
unsigned int lastTime=millis();

byte data[HABCOM_DATA_LENGTH_BYTES];
byte cDevices = 0;
HabCom::Message msg = {0,0,HabCom::CMD_NOT,0};
HabCom::Message msgHB = { HABCOM_MASTER_ADDR, HABCOM_TARGET_BROADCAST, HabCom::HEART_BEAT, 0 };

HabCom comm(RX_RO, TX_DI, HABCOM_BAUDRATE);

void setup() {
	pinMode(TXE, OUTPUT);

	comm.setBaudrate(HABCOM_BAUDRATE);
	comm.begin();

	Serial.begin(57600);

    msgHB.Data = cDevices+1;

	DbgMsg("==============================");
	DbgMsgInline("HabCom library version: ");
	DbgMsg(HABCOM_VERSION);
    DbgMsg("==============================");
	DbgMsg("Router Setup finished.");

    // blink(4, 400);

	NextSlotTime = (int)(millis()/HABCOM_CYCLE_LENGTH)*HABCOM_CYCLE_LENGTH + HABCOM_CYCLE_LENGTH;
	DbgMsg("Current Time : " + String(millis()));	//musí být na konci aby nedoslo k "pretecení" slotu
	DbgMsg("Next Slot Time : " + String(NextSlotTime));	//musí být na konci aby nedoslo k "pretecení" slotu
    while (millis()<NextSlotTime) {
        delayMicroseconds(250);
    }
}

void loop() {
    msgHB.Data = millis()-lastTime;
    comm.sendMsg(msgHB);
    delay(HABCOM_CYCLE_LENGTH);
    // CurrentTime = millis();
    // if (NextSlotTime <= CurrentTime) {
    //     if (CurrentTime - NextSlotTime <= HABCOM_MSG_START_TOLERANCE) {
    //         msgHB.Data = cDevices+1;
    //         comm.sendMsg(msgHB);
    //         // DbgMsg((byte)(CurrentTime>>10));
    //         DbgMsg(micros());
    //     } else {
    //         DbgMsg("Slot missed");
    //     }
    //     NextSlotTime = NextSlotTime + HABCOM_CYCLE_LENGTH; 
    // }
    // delayMicroseconds(250);
}
