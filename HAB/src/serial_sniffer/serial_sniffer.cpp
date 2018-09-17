#include <Arduino.h>
#include <SoftwareSerial.h>
#include "RS485_protocol.h"

#define TX 10
#define RX 11
#define DEV_ADDR 0xA1

//komentar

SoftwareSerial rs485(RX,TX);
// int data1, data2, last_data1 = 0, last_data2 =0;
// bool change = false;
// String output;

void fWrite(const byte what) {
  rs485.write(what);
}
int fAvailable() {
  return rs485.available ();
}
int fRead() {
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

void setup() {
  rs485.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("READY\n");
}

void loop() {

  byte buf [32];

  byte received = recvMsg (fAvailable, fRead, buf, sizeof (buf));

  if (received) {
    Serial.print("\nAddr: ");
    Serial.print(buf[0], HEX);
    Serial.print(", CMD: ");
    Serial.print(buf[1], HEX);
    Serial.print(", SRC: ");
    Serial.print(buf[2], HEX);
    Serial.print(", DATA: ");

    for (int di=3; di<received; di++) {
      Serial.print(buf[di], HEX);
      Serial.print(" ");
    }

   }  // end if something received

  // if (myCom.available()) {
  //   // output = "";
  //   // data = com.read();
  //   // switch (data) {
  //   //   case 02: output = "^"; break;
  //   //   case 03: output = "&\n"; break;
  //   //   default: output = String(data);
  //   // }
  //   // Serial.print(output);
  //   digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //   Serial.print(myCom.read());
  //
  // }
}
