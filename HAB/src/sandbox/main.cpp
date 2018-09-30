#include <Arduino.h>
#include "HabComProps.h"
#include "RS485_protocol.h"
#include <SoftwareSerial.h>

byte dataRaw[4] = {0b11111100,0b00001111,0b11001100,0b10101010};
byte dataRaw2[4] = {0,0,0,0};
HabComMsg msg = {0, 0, CMD_NOT, 0};
HabComMsg msg2 = {0, 0, CMD_NOT, 0};
unsigned long time = 0, time_temp = 0;

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

inline void MsgDataDecode(byte *data, HabComMsg &msg) {
  msg.Source = data[0] >> 2;
  msg.Target = (data[0] & 0b11) << 4 | data[1] >> 4;
  msg.Cmd    = (HabComCmd)(data[1] & 15);
  msg.Data   = (word)data[2] << 8 | (word)data[3];
}

inline void MsgDataEncode(byte *data, HabComMsg &msg) {
  data[0] = ((byte)msg.Source) << 2 | ((((byte)msg.Target) >> 4) & 15);
  data[1] = ((byte)msg.Target) << 4 | ((byte)msg.Cmd);
  data[2] = (byte)(msg.Data >> 8);
  data[3] = (byte)(msg.Data & 0b11111111);
}

void setup() {
    Serial.begin(9600);

    Serial.println("Ready");

    unsigned long us1 = micros();
    MsgDataDecode(dataRaw, msg);
    us1 = micros() - us1;
    Serial.println(us1);
    us1 = micros();
    MsgDataEncode(dataRaw2, msg);
    us1 = micros() - us1;
    Serial.println(us1);

    Serial.println("Msg:");
    Serial.print("   Source: ");
    Serial.println(msg.Source, BIN);
    Serial.print("   Target: ");
    Serial.println(msg.Target, BIN);
    Serial.print("   Cmd:    ");
    Serial.println(msg.Cmd, BIN);
    Serial.print("   Data:   ");
    Serial.println(msg.Data, BIN);

    Serial.print("\nData1: ");
    for (byte b = 0; b < 4; b++)
    {
        Serial.print(dataRaw[b], BIN);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.print("Data2: ");
    for (byte b = 0; b < 4; b++)
    {
        Serial.print(dataRaw2[b], BIN);
        Serial.print(" ");
    }
    Serial.println("");

    Serial.print("EQUAL? ");
    // Serial.println(
    //     (msg.Source==msg2.Source) &
    //     (msg.Target == msg2.Target) &
    //     (msg.Cmd == msg2.Cmd) &
    //     (msg.Data == msg2.Data)
    // );
    Serial.println(
        (dataRaw[0]==dataRaw2[0]) &
        (dataRaw[1]==dataRaw2[1]) &
        (dataRaw[2]==dataRaw2[2]) &
        (dataRaw[3]==dataRaw2[3])
    );

    time = millis();
}

void loop() {
    time_temp = millis();
    if (time_temp - time >= 1000) {
        time = time_temp;
        
    }
    else
    {
        delay(1);
    }
}
