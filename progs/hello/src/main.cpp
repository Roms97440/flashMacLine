#include <Arduino.h>

int cpt=0;
void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  pinMode(13,OUTPUT);
  
  
}

void loop() {
  Serial.println(cpt);
  cpt++;
  Serial.println("On");
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  Serial.println("Off");
  delay(1000);
  
  // put your main code here, to run repeatedly:
}

