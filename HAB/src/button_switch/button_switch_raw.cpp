#include <Arduino.h>
// #include "HabCom.h"
#include <clickButton.h>
#include <SoftwareSerial.h>
#include "RS485_protocol.h"

#define BTN_COUNT 2
#define DEV_ADDR 0xB1
#define RX_RO 11
#define TX_DI 12
#define TXE 10

// unsigned long i = 0;

byte data2send[] = {0xA1, 4, DEV_ADDR, 0, 2};
// byte data2send[2] = {0, HabCom::RELAY_SWITCH};

bool changed = false;

ClickButton btn[BTN_COUNT] = {
  ClickButton(4, HIGH, false),
  ClickButton(5, HIGH, false)
};

byte ledPins[] = {8, 9};

byte data[32];
byte sourceAddr=0, targetAddr=0, data_length=0;
// HabCom::CMD command = HabCom::CMD_NOT;


// HabCom COMM(28800);
SoftwareSerial rs485(RX_RO,TX_DI);
// HabCom COMM(RX, TX, 9600);

void fWrite(const byte what) {
  digitalWrite(TXE, HIGH);
  rs485.write(what);
  delay(1);
  digitalWrite(TXE, LOW);
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(TXE, OUTPUT);
  digitalWrite(TXE, LOW);

  // COMM.setDeviceAddr(DEV_ADDR);
  // COMM.useSendEnablePin(false);
  // COMM.begin();
  // COMM.sendMsg(HabCom::BROADCAST, HabCom::STATUS_INFO, (byte)HabCom::BOOTING, 1);

  rs485.begin(9600);

  for (int i=0; i < BTN_COUNT; i++) {
    //pinMode(btnPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
    btn[i].debounceTime   = 5;
    btn[i].multiclickTime = 350;
    btn[i].longClickTime  = 500;
  }

  // COMM.sendMsg(HabCom::BROADCAST, HabCom::STATUS_INFO, (byte)HabCom::READY, 1);
  blink(15,100);
}

void loop() {
  for (int i=0; i<BTN_COUNT; i++) {

    btn[i].Update();
    int func = btn[i].clicks;

    if (func == 0) continue;

    digitalWrite(ledPins[i], !digitalRead(ledPins[i]));

    if (func<0) func = (func * -1) + 4;
    data2send[3] = func + (i*8) -1;
    // data2send[0] = func + (i*8);

    sendMsg(fWrite, data2send, sizeof data2send);
    // COMM.sendMsg(0xA1, HabCom::CMD_SET, data2send, 2);
  }
  delay(5);
}
