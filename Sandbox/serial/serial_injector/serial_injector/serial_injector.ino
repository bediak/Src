#include <SoftwareSerial.h>
#define TX 2
#define RX 3
#define STX 2
#define ETX 3

SoftwareSerial com(RX, TX);

enum CMD {
  CMD_NOT,
  HEART_BEAT,
  STATUS_INFO,
  CMD_GET,
  CMD_SET
};

enum RELAY_VALUE {
  RELAY_ON,
  RELAY_OFF
};


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  com.begin(9600);
  Serial.println("READY");
}

byte data[] = { 0, RELAY_ON };

void loop() {
  // while there is data coming in, read it
  // and send to the hardware serial port:
  while (Serial.available() > 0) {
    //Serial.println("Dostupné BYTE: "+ String(Serial.available()));
    byte inByte = Serial.read();
    byte inByte2 = Serial.read();
    data[0] = inByte-65;
    switch (inByte2) {
      case 49: data[1] = RELAY_ON; break;
      case 48:
      default: data[1] = RELAY_OFF; break;
    }
    //Serial.println("BYTE1: " + String(data[0]) + ", BYTE2: " + String(data[1]));
    sendMsg(0xA1, CMD_SET, data, sizeof data);
  }
  delay(1000);
}

void sendComplemented (const byte what) {
  byte c;

  // first nibble
  c = what >> 4;
  com.write((c << 4) | (c ^ 0x0F));

  // second nibble
  c = what & 0x0F;
  com.write((c << 4) | (c ^ 0x0F));
}  // end of sendComplemented

byte crc8 (const byte *addr, byte len) {
  byte crc = 0;
  while (len--)
    {
    byte inbyte = *addr++;
    for (byte i = 8; i; i--)
      {
      byte mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix)
        crc ^= 0x8C;
      inbyte >>= 1;
      }  // end of for
    }  // end of while
  return crc;
}  // end of crc8

bool sendMsg (const byte targetAddr, const CMD command, byte * data, const byte length) {
  byte datagramm_length = length + 3;
  byte datagramm[datagramm_length];

  datagramm[0] = targetAddr;
  datagramm[1] = command;
  datagramm[2] = 01;
  memcpy(datagramm+3, data, length);

  com.write(STX);  // STX
  for (byte i = 0; i < datagramm_length; i++)
    sendComplemented(datagramm[i]);
  com.write(ETX);  // ETX
  sendComplemented(crc8 (datagramm, datagramm_length));
}  // end of sendMsg

