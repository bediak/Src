#include <Arduino.h>
#include "HabCom.h"
#include <MySoftSerial.h>

void blink(byte count /* = 3 */, int duration /* = 200 */) {
  for (byte i = 0; i<count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    digitalWrite(LED_BUILTIN, LOW);
    delay(duration/2);
  }
}

HabCom::HabCom(unsigned int speed /* = 9600 */) :
  _HwSerialUsed(true),
  _speed(speed)
{
}

HabCom::HabCom(int RX, int TX, unsigned int speed /* = 9600 */) :
  _HwSerialUsed(false),
  _speed(speed),
  _TX(TX),
  _RX(RX)
{
  _COM.setPins(RX, TX);
}

/* static  */ void HabCom::MsgDataDecode(const byte *data, HabCom::Message &msg) {
  msg.Source = data[0] >> 2;
  msg.Target = (data[0] & 0b11) << 4 | data[1] >> 4;
  msg.Cmd    = (Command)(data[1] & 15);
  msg.Data   = (word)data[2] << 8 | (word)data[3];
}

/* static */ void HabCom::MsgDataEncode(byte *data, const HabCom::Message &msg) {
  data[0] = ((byte)msg.Source) << 2 | ((((byte)msg.Target) >> 4) & 15);
  data[1] = ((byte)msg.Target) << 4 | ((byte)msg.Cmd);
  data[2] = (byte)(msg.Data >> 8);
  data[3] = (byte)(msg.Data & 0b11111111);
}

/* static */ bool HabCom::isHeartBeat(const HabCom::Message &msg) {
  return (msg.Source == HABCOM_MASTER_ADDR && msg.Target == HABCOM_TARGET_BROADCAST && msg.Cmd == HEART_BEAT);
}

void HabCom::setBaudrate(unsigned int speed) {
  _speed = speed;
}

void HabCom::setSwPins(int RX, int TX) {
  _HwSerialUsed = false;
  _TX = TX;
  _RX = RX;
  _COM.setPins(RX, TX);
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
  if (deviceAddr == HABCOM_TARGET_BROADCAST || deviceAddr == HABCOM_MASTER_ADDR)
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
    HabCom::_COM.setPins(_RX, _TX);
    HabCom::_COM.begin(_speed);
  }
  return true;
}

int HabCom::available() {
  if (_HwSerialUsed)
    return Serial.available();
  else
    return HabCom::_COM.available();

  return 0; // sem uz by se program nikdy nemel dostat, je to jen kvuli kompilatoru
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
  
  return 0; // sem uz by se program nikdy nemel dostat, je to jen kvuli kompilatoru
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

// void HabCom::println(const int number) {
//   Serial.println(number);
// }

bool HabCom::sendMsg (const HabCom::Message &msg) {
  if (millis() - _lastMsgTime < _standByTime) return false;

  if (_sendEnablePinSet) digitalWrite(_sendEnablePin, HABCOM_MODE_SENDER);

  // TODO
  // pridat moznost cekani (TIMEOUT) na odeslani zpravy
  // pridat cekani na dalsi vhodnou iteraci sem nebo do hlavni/rodicovske smycky?

  byte datagramm[HABCOM_MSG_LENGTH_BYTES];

  MsgDataEncode(datagramm, msg);

  // for (byte b=0; b<datagramm_length; b++) {
  //   Serial.print(datagramm[b],DEC);
  //   Serial.print(" ");
  // }
  // Serial.print("\n");
  //TODO: proc je tu nasledujici radek?
  // for (byte b=0; b<HABCOM_MSG_LENGTH_BYTES; b++) HabCom::_COM.write(datagramm[b]);
  HabCom::write(STX);  // STX
  for (byte i = 0; i < HABCOM_MSG_LENGTH_BYTES; i++)
    HabCom::sendComplemented(datagramm[i]);
  HabCom::write(ETX);  // ETX
  HabCom::sendComplemented(crc8 (datagramm, HABCOM_MSG_LENGTH_BYTES));

  delay(200);

  if (_sendEnablePinSet) digitalWrite(_sendEnablePin, HABCOM_MODE_LISTENER);

  //TODO: nastavit navratovou hodnotu dle uspesnosti odesilani
  return true;
}  // end of sendMsg

bool HabCom::recvMsg (HabCom::Message &msg, unsigned long timeout /* = 250 */) {
  byte data_raw[HABCOM_MSG_LENGTH_BYTES];
  byte length = recvMsgRaw(data_raw, HABCOM_MSG_LENGTH_BYTES, timeout);
  if (length==HABCOM_MSG_LENGTH_BYTES) MsgDataDecode(data_raw, msg);
  //TODO: jakou vracet delku? RAW vs. jen data?
  return length == HABCOM_DATA_LENGTH_BYTES;
}

// receive a message, maximum "length" bytes, timeout after "timeout" milliseconds
// if nothing received, or an error (eg. bad CRC, bad data) return 0
// otherwise, returns length of received data
byte HabCom::recvMsgRaw (byte * data,                    // buffer to receive into
              const byte length,              // maximum buffer size
              unsigned long timeout /* = 250 */)          // milliseconds before timing out
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
