#include <../include/Receiver.h>
#include <Arduino.h>

#define DEADBAND 25 //100 ms + or - from 1500ms is seen as stationary
#define RESTING_T_HIGH 1500

void Receiver::config_channel(int channel, int input, int output) { //Only call once during initialization
  //For now, we don't care about the output - it won't even be wired
  Receiver::channel_info[channel-1][1] = input;
  Receiver::channel_info[channel-1][2] = output;

  pinMode(input, INPUT);
  pinMode(output, OUTPUT);
}

void Receiver::connect_receiver(int ch) {
  int input = Receiver::channel_info[ch-1][0];
  int output = Receiver::channel_info[ch-1][1];
  if (input < 0 || output < 0){
    Serial.println("Aborted connection because of invalid input/output pins. Input: " + String(input) + ", Output: " + String(output));
    return;
  }

  float t_high = pulseIn(input, HIGH);
  float t_low = pulseIn(input, LOW);
  float duty_cycle = (t_high / (t_high + t_low));
  Serial.print("T-high: " + String(t_high)); // Regular t-h is 1500 ms i think (it extends to 2000 and retracts to 1000)
  Serial.print("Duty Cycle is: " + String(duty_cycle));

  float new_signal = map(duty_cycle, 0, 100, 0, 255);
  analogWrite(output, new_signal); //From 0 to 255 representing the duty cycle

  // when you go forward, time high decreases
  // when you go backward, time high increases
}

void Receiver::disable_forward(int forward_channel, int relay_pin) {
  int input = Receiver::channel_info[forward_channel-1][0];
  if (input < 0){
    Serial.println("Aborted connection because of invalid input pin at " + String(input));
    return;
  }
  float t_high = pulseIn(input, HIGH);
  
  if (t_high < (RESTING_T_HIGH-DEADBAND)){ //if you try to go forward (t_high increases) , you will reset to the resting time high
      digitalWrite(relay_pin, HIGH);
  }else if(t_high > (RESTING_T_HIGH+DEADBAND)){ // allow you to move backward
      digitalWrite(relay_pin, LOW);
  }
}

Receiver::Receiver(){
  Serial.println("Initialized Receiver");
}
