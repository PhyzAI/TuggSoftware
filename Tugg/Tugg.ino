#include <Ultrasonic.h>

Ultrasonic ultrasonic(11, 10); //trigger, and echo
//Running on an Arduino Nano

void setup() {
  // put your setup code here, to run once:
  //TODO: if the sensor is touching something else, it will output really high, incorrect numbers

  Serial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(2, LOW);
  //We want pin 2 to be HIGH regularly, and pin 2 to be LOW if it gets too close
  //This is because a high signal will activate the relay which will keep the switches active. When a low signal it sent, the switches will release, and power will be cut to the motors
}


void loop() {
  // put your main code here, to run repeatedly:
  int dist = ultrasonic.read(CM);
  Serial.println(dist);

  if (dist < 50){
    Serial.println("Too Close!!!!");
    digitalWrite(LED_BUILTIN, HIGH);    
    digitalWrite(2, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(2, LOW);
  }
  delay(100);
}
