#include <SoftwareSerial.h>
#include <Wire.h>

#define ttp229 (0xAF>>1)
#define TX 6
#define RX 7

#define bus_RX 10
#define bus_TX 11
#define bus_CTRL 3

uint16_t data_out = 0;
uint16_t data1, data2;
int count = 0;
int button[16];
int oldata[16];
bool new_line = false;

SoftwareSerial COM(RX, TX);

SoftwareSerial BUS(bus_RX, bus_TX);

void setup()
{
  pinMode(bus_CTRL, OUTPUT);
  Serial.begin(9600);
  COM.begin(9600);
  BUS.begin(14400);
  Wire.begin();
  Serial.println("Buttons ready!");
}

void busDataSend(SoftwareSerial &bus, int &btn, int CTRL_PIN = bus_CTRL)
{
  //allow data trasnmitting
  digitalWrite(CTRL_PIN, HIGH);

  bus.write(btn);
  bus.write(0x0A);

  //disallow data trasnmitting
  digitalWrite(CTRL_PIN, LOW);
}

void loop()
{
  Wire.requestFrom(ttp229, 2, true);
  delay(250);
  while (Wire.available())
  {
    data1 = Wire.read();
    data2 = Wire.read();
    data_out = (data1 << 8) | data2;

    for (int i = 1; i < 17; i++)
    {
      uint16_t contrast = 0x8000;
      if (data_out & contrast)
      {
        button[count] = i;
        count++;
        delay(1);
      }
      data_out <<= 1;
    }
    if (count>0)
    {
      for (int i = 0; i < count; i++)
      {
        new_line = false;
        if (button[i] == oldata[i])
        {
          new_line = true;
          Serial.print(button[i]);
          if ((i+1)<count) Serial.write(" ");
          //COM.write(button[i]);
          COM.print(button[i]);COM.print('\n');
          busDataSend(BUS, button[i]);
          delay(50);
        }
      }
      if (new_line) Serial.println("");
      for (int i = 0; i < 16; i++)
      {
        oldata[i] = button[i];
      }
      //reset
      count = 0;
    }
  }
}

