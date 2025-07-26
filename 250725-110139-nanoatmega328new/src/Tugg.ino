#include <Ultrasonic.h>
#include <../include/Receiver.h>
#include <Arduino.h>

// Running on an Arduino Nano
//gcc -v -E -x c++ <-- cmd to get c++ compiler locations on pc
#define RELAY_SWITCH_PIN 2
#define ULTRASONIC_ECHO_PIN 10
#define ULTRASONIC_TRIGGER_PIN 11

#define STOP_DISTANCE 50 // cms
#define FILTER_THRESHOLD 10 //cms <-- this value or less should be the difference between each scan

int last_val = -1; //last ultrasonic sensor reading

Receiver receiver;
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

void config_pins(){
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
  TCCR1B = (1 << WGM13) | (1 << CS11) | (1 << CS10); // Phase correct PWM, prescaler 64

  ICR1 = 12500; // Set TOP for 10 Hz: 16MHz / (2 * 64 * 10)
  OCR1A = 0;     // default duty cycle of 0
}

void setup(){
  Serial.begin(9600);
  
  config_pins();
  config_timer();

  receiver.config_channel(1, 3, 9); //Channel 1, pin 3 is input, and pin 9 output (motor forward-backward)
  // receiver.config_channel(2, 5, 6); // motor left right 

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY_SWITCH_PIN, LOW);
  // We want pin 2 to be LOW regularly, and pin 2 to be HIGH if it gets too close
  // This is because a high signal will activate the relay which will keep the switches active. When a low signal it sent, the switches will release, and power will be cut to the motors
}



void loop(){
  receiver.connect_receiver(1); // reads the signal in, and reconstructs with the same duty cycle/frequency
  // the ch1 output is motor forward and backward, (the mdds will convert the wave into directions for the motor) we don't need ch2

  int dist = filter_read(ultrasonic);
  if (dist==-1){return;} //whack value reading, ignore it (warning message will be sent though)
  
  if (dist < STOP_DISTANCE){
    Serial.println("Too Close!!!!");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(RELAY_SWITCH_PIN, HIGH); 
    receiver.disable_fwd = true; // Disable forward movement
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(RELAY_SWITCH_PIN, LOW);
    receiver.disable_fwd = false;
  }

  // TODO use milis to add a delay and acount for the delay in the read filter
}

int filter_read(Ultrasonic ult){
  int now_val = ult.read(CM);
  if (last_val == -1){last_val = now_val;}

  if (abs(now_val - last_val) >= FILTER_THRESHOLD){
    Serial.println("WARNING: An abnormal reading was seen when scanning with the ultrasonic sensor, please check that everything is connected correctly");
    last_val = now_val;
    return -1;
  }

  last_val = now_val;
  return now_val;
}