#include <../include/Receiver.h>
#include <Arduino.h>

#define DEADBAND 25 //100 ms + or - from 1500ms is seen as stationary
#define RESTING_T_HIGH 1500

void Receiver::config_channel(int channel, int input, int output) { //Only call once during initialization
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

  if ((ch == 1) && disable_fwd){ //only applicable to channel 1
    if (t_high < (RESTING_T_HIGH-DEADBAND)){ //if you try to go forward , you will reset to the resting time high
      duty_cycle = RESTING_T_HIGH;
    }
  }

  //Depending on if this approach of modifying the timer stuff is needed or not, I may rewrite the connect_receiver function, as right now it will only truly work for channel 1 with an output pin of 9
  OCR1A = ICR1 * duty_cycle; //sets the duty cycle for PIN 9 SPECIFICALLY
  // when you go forward, time high decreases
  // when you go backward, time high increases
}

Receiver::Receiver(){
  Serial.println("Initialized Receiver");
}
