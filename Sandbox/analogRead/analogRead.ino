#include <SoftwareSerial.h>

SoftwareSerial COM(A7, A6);

void setup() {
  Serial.begin(9600);
  COM.begin(9600);

  Serial.println("START");
}

double val = 0;
double newVal = 0;

void loop() {
//  if (COM.available() > 0) 
//  {
//    Serial.println(COM.read());
//  }
//
    newVal = analogRead(A7);
    if (newVal != val)
    {
      Serial.println(newVal);
      
    }
}

