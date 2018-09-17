#include <SoftwareSerial.h> // inicializijeme knihovnu
 
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin
 
#define SSerialTxControl 3   // RS modul pin 3
 
// vytvorime seriovy port na pinu 10 a 11 se jménem RS485Serial
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

bool b = 0;
 
void setup()  
{
  // Inicializujeme seriový port na klasickém pin 0 a 1 (TX a RX)
  Serial.begin(9600);
 
  // nastartujeme software serial
  RS485Serial.begin(9600);
  
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, 1);  // Povolí odesílání na modulu RS485    
}
 
 
void loop()  { 
//    RS485Serial.write("arduino8.webnode.cz"); // odešle text
    RS485Serial.write(b+65);
//    RS485Serial.write(0x0A); // odešle znak kového řádku      
    RS485Serial.write("\n"); // odešle znak kového řádku      
    b = !b;
//    digitalWrite(SSerialTxControl, 0);  // Zakáže odesílání na modulu RS485
    delay(1);  
}
