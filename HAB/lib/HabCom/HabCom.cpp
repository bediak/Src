#include <Arduino.h>
#include "HabCom.h"
#include <SoftwareSerial.h>

HabCom::HabCom(unsigned int speed = 9600) {
  _HwSerialUsed = true;
  _speed = speed;
}

HabCom::HabCom(int RX, int TX, unsigned int speed = 9600) {
  _HwSerialUsed = false;
  _speed = speed;
  _TX = TX;
  _RX = RX;
}

void HabCom::setBaudrate(unsigned int speed) {
  _speed = speed;
}

void HabCom::setSwPins(int RX, int TX) {
  _HwSerialUsed = false;
  _TX = TX;
  _RX = RX;
}

void HabCom::setSendEnablePin(int pinNumber) {
  _sendEnablePin = pinNumber;
  _sendEnablePinSet = true;
  pinMode(_sendEnablePin, OUTPUT);
  digitalWrite(_sendEnablePin, LOW);
}

void HabCom::useSendEnablePin(bool value) {
    _sendEnablePinSet = value;
}

void HabCom::setStandByTime(unsigned long milliseconds) {
  _standByTime = milliseconds;
}

bool HabCom::setDeviceAddr(const byte deviceAddr) {
  if (deviceAddr == BROADCAST || deviceAddr == MASTER)
    return false;
  else if (deviceAddr <= 0 || deviceAddr >= 0xFF)
    return false;
  else {
    _deviceAddr = deviceAddr;
    return true;
  }
  return false;
}

bool HabCom::begin() {
  if (_HwSerialUsed) {
    Serial.begin(_speed);
  }
  else {
    HabCom::_COM = SoftwareSerial(_RX, _TX);
    HabCom::_COM.begin(_speed);
  }
  return true;
}

int HabCom::available() {
  if (_HwSerialUsed)
    return Serial.available();
  else
    return HabCom::_COM.available();
}

int HabCom::read() {
  int data = -1;
  if (_HwSerialUsed)
    data = Serial.read();
  else
    data = HabCom::_COM.read();

  if (data != -1) _lastMsgTime = millis();

  return data;
}

byte HabCom::write(byte what) {
  if (_HwSerialUsed)
    return Serial.write(what);
  else
    return HabCom::_COM.write(what);
}

// calculate 8-bit CRC
byte HabCom::crc8 (const byte *addr, byte len) {
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

// send a byte complemented, repeated
// only values sent would be (in hex):
//   0F, 1E, 2D, 3C, 4B, 5A, 69, 78, 87, 96, A5, B4, C3, D2, E1, F0
void HabCom::sendComplemented (const byte what) {
  byte c;

  // first nibble
  c = what >> 4;
  HabCom::write((c << 4) | (c ^ 0x0F));

  // second nibble
  c = what & 0x0F;
  HabCom::write((c << 4) | (c ^ 0x0F));
}  // end of sendComplemented

static void HabCom::blink(byte count = 3, int duration = 200) {
  for (byte i = 0; i<count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    digitalWrite(LED_BUILTIN, LOW);
    delay(duration/2);
  }
}

// void HabCom::println(const int number) {
//   Serial.println(number);
// }

bool HabCom::sendMsg (const byte targetAddr, const CMD command, byte * data, const byte length) {
  if (millis() - _lastMsgTime < _standByTime) return false;

  if (_sendEnablePinSet) digitalWrite(_sendEnablePin, HIGH);

  // TODO
  // pridat moznost cekani (TIMEOUT) na odeslani zpravy
  // s nahodnym intervalem cekani

  byte datagramm_length = length + 3;
  byte datagramm[datagramm_length];

  datagramm[0] = targetAddr;
  datagramm[1] = command;
  datagramm[2] =_deviceAddr;
  // for (int bi = 0; bi < length; bi++) datagramm[bi+3] = data[bi];
  memcpy(datagramm+3, data, length);


  // for (byte b=0; b<datagramm_length; b++) {
  //   Serial.print(datagramm[b],DEC);
  //   Serial.print(" ");
  // }
  // Serial.print("\n");
  for (byte b=0; b<datagramm_length; b++) HabCom::_COM.write(datagramm[b]);
  HabCom::write(STX);  // STX
  for (byte i = 0; i < datagramm_length; i++)
    HabCom::sendComplemented(datagramm[i]);
  HabCom::write(ETX);  // ETX
  HabCom::sendComplemented(crc8 (datagramm, datagramm_length));

  if (_sendEnablePinSet) digitalWrite(_sendEnablePin, LOW);
}  // end of sendMsg

byte HabCom::recvMsg (byte & targetAddr, CMD & command, byte & sourceAddr, byte * data, unsigned long timeout = 250) {
  byte data_raw[HABCOM_MSG_LENGTH];
  byte length = recvMsgRaw(data_raw, HABCOM_MSG_LENGTH, timeout);
  targetAddr = data_raw[0];
  command = data_raw[1];
  sourceAddr = data_raw[2];
  memcpy(data, data_raw + 3, length - 3);
  return length - 3;
}

// receive a message, maximum "length" bytes, timeout after "timeout" milliseconds
// if nothing received, or an error (eg. bad CRC, bad data) return 0
// otherwise, returns length of received data
byte HabCom::recvMsgRaw (byte * data,                    // buffer to receive into
              const byte length,              // maximum buffer size
              unsigned long timeout = 250)          // milliseconds before timing out
  {

  unsigned long start_time = millis ();

  bool have_stx = false;

  // variables below are set when we get an STX
  bool have_etx;
  byte input_pos;
  bool first_nibble;
  byte current_byte;

  while (millis () - start_time < timeout)
    {
    if (HabCom::available() > 0)
      {
      byte inByte = HabCom::read();
      switch (inByte)
        {

        case STX:   // start of text

          have_stx = true;
          have_etx = false;
          input_pos = 0;
          first_nibble = true;
          start_time = millis ();  // reset timeout period
          break;

        case ETX:   // end of text
          have_etx = true;
          break;

        default:
          // wait until packet officially starts
          if (!have_stx)
            break;

          // check byte is in valid form (4 bits followed by 4 bits complemented)
          if ((inByte >> 4) != ((inByte & 0x0F) ^ 0x0F) )
            return 0;  // bad character

          // convert back
          inByte >>= 4;

          // high-order nibble?
          if (first_nibble)
            {
            current_byte = inByte;
            first_nibble = false;
            break;
            }  // end of first nibble

          // low-order nibble
          current_byte <<= 4;
          current_byte |= inByte;
          first_nibble = true;

          // if we have the ETX this must be the CRC
          if (have_etx)
            {
            if (HabCom::crc8(data, input_pos) != current_byte)
              return 0;  // bad crc
            return input_pos;  // return received length
            }  // end if have ETX already

          // keep adding if not full
          if (input_pos < length)
            data [input_pos++] = current_byte;
          else
            return 0;  // overflow
          break;

        }  // end of switch
      }  // end of incoming data
    } // end of while not timed out

  return 0;  // timeout
} // end of recvMsg
