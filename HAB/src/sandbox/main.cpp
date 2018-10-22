#include <Arduino.h>
#include "HabCom.h"

#define TX 10
#define RX 11

HabCom hc(RX, TX, HABCOM_BAUDRATE);
HabCom::Message msg = {0,0,HabCom::CMD_NOT,0};
bool first_idle = false;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(57600);
    hc.begin();

    Serial.println("=============================");
    Serial.print("HabCom library version: ");
    Serial.println(HABCOM_VERSION);
    Serial.println("=============================");
    // blink(4, 400);
    Serial.println("Setup finished.");

}

void loop() {
    // if (millis()%1000 == 0) Serial.println("Nuda!");
    Serial.println(hc.update());
    if (hc.update()) {
        // hc.getMsg(msg);
        Serial.print(msg.Data, DEC);
        Serial.print(".");
        first_idle = true;
    }
    else
    {
        if (first_idle) {
            Serial.println("\nNIX");
            first_idle = false;
        }
        delay(100);
    }
}
