#include <Arduino.h>
#include "HabComProps.h"

byte dataRaw[4] = {0b11010110,0b00111010,0b01010101,0b01010101};
HabComMsg msg = {0, 0, CMD_NOT, 0};
HabComMsg msg2 = {0, 0, CMD_NOT, 0};

void MsgDataParse(byte *dataRAW, HabComMsg &msg) {
    unsigned long dataComplete =  (unsigned long)dataRAW[0] << 24
                              | (unsigned long)dataRAW[1] << 16
                              | (unsigned long)dataRAW[2] << 8
                              | (unsigned long)dataRAW[3];

    msg.Data = dataComplete & 65535;
    msg.Source = dataComplete >> 26;
    msg.Target = (dataComplete >> 20) & 63;
    msg.Cmd = (HabComCmd)((dataComplete >> 16) & 15);
}

void MsgDataParse2(byte *dataRAW, HabComMsg &msg) {
    // unsigned long dataComplete =  (unsigned long)dataRAW[0] << 24
    //                           | (unsigned long)dataRAW[1] << 16
    //                           | (unsigned long)dataRAW[2] << 8
    //                           | (unsigned long)dataRAW[3];

    msg.Source = dataRaw[0] >> 2;
    msg.Target = (dataRaw[0] & 0b11) << 4 | dataRaw[1] >> 4;
    msg.Cmd    = (HabComCmd)(dataRaw[1] & 15);
    msg.Data   = (word)dataRaw[2] << 8 | (word)dataRaw[3];
}


void setup() {
    Serial.begin(9600);

    Serial.println("Ready");

    Serial.println(sizeof(CMD_NOT));

    unsigned long us1 = micros();
    MsgDataParse(dataRaw, msg);
    us1 = micros() - us1;

    unsigned long us2 = micros();
    MsgDataParse2(dataRaw, msg2);
    us2 = micros() - us2;

    Serial.print("Time1: ");
    Serial.println(us1);
    Serial.print("Time2: ");
    Serial.println(us2);

    Serial.println(msg2.Source, BIN);
    Serial.println(msg2.Target, BIN);
    Serial.println(msg2.Cmd, BIN);
    Serial.println(msg2.Data, BIN);

    Serial.print("EQUAL? ");
    Serial.println(
        (msg.Source==msg2.Source) &
        (msg.Target == msg2.Target) &
        (msg.Cmd == msg2.Cmd) &
        (msg.Data == msg2.Data)
    );


}

void loop() {
    delay(1000);
}
