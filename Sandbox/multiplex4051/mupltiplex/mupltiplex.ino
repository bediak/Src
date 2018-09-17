#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define PinReset 7
#define PinCtrl 8
#define BitCount 3
#define PinCount 8

enum RelayStateValues { ON, OFF };

const byte BitPins[BitCount] = {A0, A1, A2};
const byte PinMap[PinCount] = {0, 1, 2, 3, 4, 5, 6, 7};
RelayStateValues RelayStates[PinCount];


void resetRelays() {
  digitalWrite(PinCtrl, RELAY_OFF);
  for (int i = 0; i < PinCount; i++ ) {
    for (int j = 0; j< BitCount; j++) {
      digitalWrite(BitPins[i], bitRead(i, j));
      digitalWrite(PinReset, LOW);
      delay(5);
      digitalWrite(PinReset, HIGH);
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(PinReset, OUTPUT);
  pinMode(PinCtrl, OUTPUT);

  digitalWrite(PinCtrl, RELAY_OFF);

  for (int i = 0; i< BitCount; i++) {
    pinMode(BitPins[i], OUTPUT);
    Serial.println("Nastavuji BitPin " + String(BitPins[i]) + " jako OUTPUT.");
  }

  for (int i = 0; i < PinCount; i++ ) {
    Serial.println("Resetuji relay " + String(PinMap[i]) + "...");
    RelayStates[i] = RELAY_OFF;
    for (int j = 0; j< BitCount; j++) {
      // Serial.println("...bit " + String(j) + " < " + String(bitRead(i, j)) + "...");
      digitalWrite(BitPins[i], bitRead(i, j));
    }
  }

  digitalWrite(PinReset, HIGH);

  Serial.println("\nHotovo.\n");
}

void loop() {
    
  // příjem zprávy po seriove komunikaci
  if (Serial.available()) {
    String relay = Serial.readString();
    int relay_number = relay.toInt();
    //Serial.println("Dostal jsem "+ String(relay_number));

    // zmena stavu rele v seznamu - pokud v rozsahu
    if (relay_number >= 0 && relay_number < PinCount) {
      //Serial.println("Prepinam rele "+ relay);
      if (RelayStates[relay_number] != RELAY_OFF)
        RelayStates[relay_number] = RELAY_OFF;
      else
        RelayStates[relay_number] = RELAY_ON;
      //vypis stavu
//      for (int ri = 0; ri < PinCount; ri++) Serial.print(RelayStates[ri]);
//      Serial.println(".");
      resetRelays();
      delay(50);
    }
  }

  //aktualizace rele dle stavu v seznamu
  for (int ri = 0; ri < PinCount; ri++ ) {
    if (RelayStates[ri]==RELAY_ON) {
      for (int bi = 0; bi< BitCount; bi++) {
        digitalWrite(BitPins[bi], bitRead(ri, bi));
        digitalWrite(PinReset, LOW);
        if (digitalRead(PinCtrl) != RELAY_ON) {
          digitalWrite(PinCtrl, RELAY_ON);
          delay(5);
        }
        digitalWrite(PinReset, HIGH);
      }
    }
  }
}

