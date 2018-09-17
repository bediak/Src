#include <SoftwareSerial.h>

#include <Wire.h>
#define ttp229 (0xAF>>1)
#define TX 6
#define RX 7

uint16_t data_out = 0;
uint16_t data1, data2;
int count = 0;
boolean count2 = false;
int button[16];
int oldata[16];

SoftwareSerial COM(RX, TX);

void setup()
{
  Serial.begin(9600);
  COM.begin(9600);
  Wire.begin();
  Serial.println("Buttons ready!");
}
void loop()
{
  Wire.requestFrom(ttp229, 2, true);
  delay(1);
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
    
    if (count2)
    {
      switch (count)
      {
        case 1:
          if (oldata[0] == button[0])
          {
            Serial.println(button[0], DEC);
            COM.println(button[0], HEX);
            delay(20);
          }
          break;
        case 2:
          if (oldata[0] == button[0] && oldata[1] == button[1])
          {
            for (int i = 0; i < 2; i++)
            {
              Serial.print(button[i]);
              COM.write(button[i]);
              delay(10);
            }
            Serial.println("");
          }
          break;
      }
    }
    delay(50);
    for (int i = 0; i < 16; i++)
    {
      oldata[i] = button[i];
    }
    count = 0;
    count2 = !count2;
  }
}

