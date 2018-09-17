#include <SoftwareSerial.h>

#define DO_COUNT 16
#define DEV_ID '1'
#define DATAGRAM_LENGTH 8
#define RX 2
#define TX 3


int digouts[DO_COUNT] = {4, 5, 6, 7, 8, 9, 10, 11, 12, A0, A1, A2, A3, A4, A5, 3};

//char data[DATAGRAM_LENGTH];

byte data = 0;

enum do_value
{
  low,
  high
};

int data_read = 0;
int pinNumber = 0;
int pinValue = LOW;

SoftwareSerial COM(RX, TX);

void setup() {
  Serial.begin(9600);
  COM.begin(9600);

  Serial.println("Start...");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  for (int i=0; i < DO_COUNT; i++)
  {
    //delay(250);
    Serial.print("Init PIN");
    Serial.print(i+1, DEC);
    Serial.print("...");
    pinMode(digouts[i], OUTPUT);
    //digitalWrite(digouts[i], LOW);
    //delay(250);
    digitalWrite(digouts[i], HIGH);
    Serial.println("OK");
  }
  Serial.println("\nREADY!");
}

void loop() {
  if (Serial.available() > 0)
  {
//    data_read = Serial.readBytesUntil('\n', data, DATAGRAM_LENGTH);

    data = Serial.read();
    
    Serial.print("Recieved data ");
    Serial.println(data);

    pinNumber = data - 1;
    
    digitalWrite(digouts[pinNumber], !digitalRead(digouts[pinNumber]));
    
//    Serial.println("...");
//    Serial.print("Addr ");
//    Serial.print(data[0]);
//    Serial.print("...");
//    if (data[0] == DEV_ID)
//    {
//      Serial.println("JDU NA TO");
//      Serial.print("PIN ");
//      if (isDigit(data[1]))
//      {
//        //pinNumber = atoi(data[1]);
//        pinNumber = String(data[1]).toInt();
//        if (pinNumber < 1 || pinNumber > DO_COUNT)
//        {
//          Serial.println("out of range. END!");
//        }
//        else
//        {
//          Serial.print(pinNumber--, DEC);
//          //Serial.print(digouts[pinNumber],DEC);
//          Serial.print(" set to ");
//          if (data[2]=='0')
//          {
//            Serial.println("OFF");
//            digitalWrite(digouts[pinNumber], HIGH);
//          }
//          else if (data[2]=='1')
//          {
//            Serial.println("ON");
//            digitalWrite(digouts[pinNumber], LOW);
//          }
//          else
//          {
//            Serial.println("<err>. END!");
//          }
//        }
//      }
//      else
//      {
//        Serial.println("PIN_DATA_ERR");
//      }
//    }
//    else
//    {
//      Serial.println("IGNORUJI");
//    }
//    
//    
//    
  }
}
