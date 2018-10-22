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

/* static	*/ void HabCom::MsgDataDecode(const byte *data, HabCom::Message &msg) {
	msg.Source = data[0] >> 2;
	msg.Target = (data[0] & 0b11) << 4 | data[1] >> 4;
	msg.Cmd		= (Command)(data[1] & 15);
	msg.Data	 = (word)data[2] << 8 | (word)data[3];
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
	data_ = (byte *) malloc (HABCOM_DATA_LENGTH_BYTES);
	reset();
	errorCount_ = 0;
	return true;
}

void HabCom::stop() {
	reset();
	free (data_);
	data_ = NULL;
}

void HabCom::reset () {
	haveSTX_ = false;
	available_ = false;
	inputPos_ = 0;
	startTime_ = 0;
}



int HabCom::fAvailable() {
	if (_HwSerialUsed)
		return Serial.available();
	else
		return HabCom::_COM.available();

	return 0; // sem uz by se program nikdy nemel dostat, je to jen kvuli kompilatoru
}

int HabCom::fRead() {
	int data = -1;
	if (_HwSerialUsed)
		data = Serial.read();
	else
		data = HabCom::_COM.read();

	if (data != -1) _lastMsgTime = millis();

	return data;
}

size_t HabCom::fWrite(const byte what) {
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
			}	// end of for
		}	// end of while
	return crc;
}	// end of crc8

// send a byte complemented, repeated
// only values sent would be (in hex):
//	 0F, 1E, 2D, 3C, 4B, 5A, 69, 78, 87, 96, A5, B4, C3, D2, E1, F0
void HabCom::sendComplemented (const byte what) {
	byte c;

	// first nibble
	c = what >> 4;
	HabCom::fWrite((c << 4) | (c ^ 0x0F));

	// second nibble
	c = what & 0x0F;
	HabCom::fWrite((c << 4) | (c ^ 0x0F));
}	// end of sendComplemented

// void HabCom::println(const int number) {
//	 Serial.println(number);
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
	//	 Serial.print(datagramm[b],DEC);
	//	 Serial.print(" ");
	// }
	// Serial.print("\n");
	//TODO: proc je tu nasledujici radek?
	// for (byte b=0; b<HABCOM_MSG_LENGTH_BYTES; b++) HabCom::_COM.write(datagramm[b]);
	HabCom::fWrite(STX);	// STX
	for (byte i = 0; i < HABCOM_MSG_LENGTH_BYTES; i++)
		HabCom::sendComplemented(datagramm[i]);
	HabCom::fWrite(ETX);	// ETX
	HabCom::sendComplemented(crc8 (datagramm, HABCOM_MSG_LENGTH_BYTES));

	delayMicroseconds(200);

	if (_sendEnablePinSet) digitalWrite(_sendEnablePin, HABCOM_MODE_LISTENER);

	//TODO: nastavit navratovou hodnotu dle uspesnosti odesilani
	return true;
}	// end of sendMsg

void HabCom::getMsg (HabCom::Message &msg) {
	MsgDataDecode(data_, msg);
	//TODO: jakou vracet delku? RAW vs. jen data?
}

bool HabCom::update () {
	// no data? can't go ahead (eg. begin() not called)
	if (data_ == NULL)
		return false;

	while (HabCom::fAvailable () > 0) {
		byte inByte = fRead ();

		switch (inByte) {

				case STX:	 // start of text
					haveSTX_ = true;
					haveETX_ = false;
					inputPos_ = 0;
					firstNibble_ = true;
					startTime_ = millis ();
					break;

				case ETX:	 // end of text (now expect the CRC check)
					haveETX_ = true;
					break;

				default:
					// wait until packet officially starts
					if (!haveSTX_)
						break;

					// check byte is in valid form (4 bits followed by 4 bits complemented)
					if ((inByte >> 4) != ((inByte & 0x0F) ^ 0x0F)) {
						reset ();
						errorCount_++;
						break;	// bad character
					} // end if bad byte

					// convert back
					inByte >>= 4;

					// high-order nibble?
					if (firstNibble_) {
						currentByte_ = inByte;
						firstNibble_ = false;
						break;
					}	// end of first nibble

					// low-order nibble
					currentByte_ <<= 4;
					currentByte_ |= inByte;
					firstNibble_ = true;

					// if we have the ETX this must be the CRC
					if (haveETX_) {
						if (crc8 (data_, inputPos_) != currentByte_) {
							reset ();
							errorCount_++;
							break;	// bad crc
						} // end of bad CRC

						available_ = true;
						return true;	// show data ready
					}	// end if have ETX already

					// keep adding if not full
					if (inputPos_ < HABCOM_DATA_LENGTH_BYTES)
						data_ [inputPos_++] = currentByte_;
					else {
						reset (); // overflow, start again
						errorCount_++;
					}

					break;
			}	// end of switch
	}	// end of while incoming data

	return false;	// not ready yet
} // end of RS485::update
