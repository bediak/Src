#include <SoftwareSerial.h>
 
#define bus_RX 10
#define bus_TX 11
#define bus_CTRL 3
 
 
// vytvorime seriovy port na pinu 10 a 11
SoftwareSerial BUS(bus_RX, bus_TX);

byte byteReceived;
 
void setup()  
{
  // Inicializujeme seriový port na klasickém pin 0 a 1 (TX a RX)
  Serial.begin(9600);  
 
  pinMode(bus_CTRL, OUTPUT);  
  digitalWrite(bus_CTRL, 0);  // Povolí přijímaní na modulu RS485
 
 
  BUS.begin(9600); // rychlost RS485 musí být nastavená stejně jako na vysílači  
  
  Serial.println("Receiver ready!");
}
 
 
void loop()  {
 
   // když jsou nějaká data přijata na RS485
   while (BUS.available()>0)  
   {    
    byteReceived = BUS.read();    // Přečte byte z modulu RS485
    Serial.write(byteReceived);           // a odešle je na Serial Monitor
   }
   Serial.println("nic");
   delay(50);

}

