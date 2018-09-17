#define led 13
void setup() {
 pinMode(led,OUTPUT);
 digitalWrite(led,LOW);
}

void loop() {
  delay(6000);
  digitalWrite(led,HIGH);

}
