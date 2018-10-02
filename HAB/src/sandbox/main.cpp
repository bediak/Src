#include <Arduino.h>
// #include <SoftwareSerial.h>
#include "HabCom.h"

#define TX 10
#define RX 11

// SoftwareSerial comm(RX, TX);
HabCom hc(RX, TX, HABCOM_BAUDRATE);
HabCom::Message msg = {0,0,HabCom::CMD_NOT,0};
bool first_idle = false;

void setup() {
    Serial.begin(57600);
    hc.begin();
    // comm.begin(57600);

    Serial.println("=============================");
    Serial.print("HabCom library version: ");
    Serial.println(HABCOM_VERSION);
    Serial.println("=============================");
    blink(4, 400);
    Serial.println("Setup finished.");

}

void loop() {
    if (hc.available()) {
        hc.recvMsg(msg);
        Serial.print(msg.Data, DEC);
        Serial.print(" ");
        first_idle = true;
    }
    else
    {
        if (first_idle) {
            Serial.println();
            first_idle = false;
        }
        delay(100);
    }
}
