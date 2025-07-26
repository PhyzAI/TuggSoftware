#include <Ultrasonic.h>
#include <Receiver.h>
#include <Arduino.h>

// Running on an Arduino Nano

#define RELAY_SWITCH_PIN 2
#define ULTRASONIC_ECHO_PIN 10
#define ULTRASONIC_TRIGGER_PIN 11

#define STOP_DISTANCE 50 // cm

#define RECEIVER_CH1_INPUT 3 // motor forward backward
#define RECEIVER_CH2_INPUT 5 // motor left right

#define RECEIVER_CH1_OUTPUT 9
#define RECEIVER_CH2_OUTPUT 6

Receiver receiver;
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

void config_pins()
{
  pinMode(RECEIVER_CH1_INPUT, INPUT);
  pinMode(RECEIVER_CH2_INPUT, INPUT);

  pinMode(RECEIVER_CH1_OUTPUT, OUTPUT);
  pinMode(RECEIVER_CH2_OUTPUT, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_SWITCH_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT_PULLUP); // if nothing is connected set input to high by default
}

void config_timer() { // changing the frequency for Timer1 which is connected to pin 9 and 10 - Code courtesy of ChatGPT
  //Stops timer1
  TCCR1A = 0; 
  TCCR1B = 0; 

  // Set Timer1 to Phase Correct PWM with ICR1 as TOP
  TCCR1A = (1 << COM1A1);              // Non-inverting mode on OC1A
  TCCR1B = (1 << WGM13) | (1 << CS11); // Phase correct PWM, prescaler 8

  ICR1 = 100000; // Set TOP for 10 Hz: 16MHz / (2 * 8 * 10)
  OCR1A = 0;     // default duty cycle of 0
}

void setup(){
  // TODO: if the sensor is touching something else, it will output really high, incorrect numbers
  Serial.begin(9600);
  
  config_pins();
  config_timer();

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY_SWITCH_PIN, LOW);
  // We want pin 2 to be LOW regularly, and pin 2 to be HIGH if it gets too close
  // This is because a high signal will activate the relay which will keep the switches active. When a low signal it sent, the switches will release, and power will be cut to the motors
}



void loop(){
  // connect_receiver(RECEIVER_CH1_INPUT, RECEIVER_CH1_OUTPUT); // reads the signal in, and reconstructs with the same duty cycle
  // the ch1 output is motor forward and backward, (the mdds will convert the wave into directions for the motor) we don't need t
  

  int dist = ultrasonic.read(CM); // TODO what to do when getting a whack value, add a filter to track the change in values, and have it in a reasonable range
  if (dist < STOP_DISTANCE){
    Serial.println("Too Close!!!!");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(RELAY_SWITCH_PIN, HIGH); // Disable forward movement
    
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(RELAY_SWITCH_PIN, LOW);
  }

  // TODO use milis to add a delay and acount for the delay in the read filter
}
