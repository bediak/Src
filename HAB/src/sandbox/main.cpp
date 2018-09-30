#include <Arduino.h>
#include "HabCom.h"
#include "RS485_protocol.h"
#include <SoftwareSerial.h>

byte dataRaw[4] = {0b11111100,0b00001111,0b11001100,0b10101010};
byte dataRaw2[4] = {0,0,0,0};
HabCom::Message msg = {0, 0, HabCom::CMD_NOT, 0};
unsigned long time = 0, time_temp = 0;

void setup() {
    Serial.begin(9600);

    Serial.println("Ready");

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
