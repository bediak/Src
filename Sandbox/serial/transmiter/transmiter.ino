

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  Serial.println("Serial transmiter");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()>0)
  {
    Serial.println(Serial.readStringUntil('\n'));
  }
}
