/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#include <math.h>

#define LED_COUNT 3

const byte led_pins[LED_COUNT] = {11,12,13};


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  for (int i = 0; i < LED_COUNT; i++) pinMode(led_pins[i], OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  for (int i = 1; i < pow(LED_COUNT, 2); i++) { //začínám od jedné pro vynechání nuly=zhasnuto)
    //Serial.print("\n");
    //Serial.print(i);
    //Serial.print("...");
    for (int j = 0; j <LED_COUNT; j++) {
      byte val = bitRead(i, j);
      digitalWrite(led_pins[j], val);   // turn the LED on (HIGH is the voltage level)
      //Serial.print(led_pins[j]);
      //Serial.print("->");
      //Serial.print(val);
      //Serial.print(", ");
    }
    delay(100);                       // wait for a second
    for (int i = 0; i < LED_COUNT; i++) digitalWrite(led_pins[i], LOW);
  }
}
