#include <Receiver.h>
#include <Arduino.h>


void Receiver::config_channel(int channel, int pins[2]) {
    Receiver::channel_info.push_back({channel, pins[0], pins[1]});
    
}

void Receiver::connect_receiver(int input_ch, int output_ch) {
    
  float t_high = pulseIn(input_ch, HIGH);
  float t_low = pulseIn(input_ch, LOW);
  float duty_cycle = (t_high / (t_high + t_low)) * 100;
  Serial.print("T-high: ");
  Serial.println(t_high); // Regular t-h is 1500 ms i think (it extends to 2000 and retracts to 1000)
  Serial.print("Duty Cycle is: " + String(duty_cycle));
  OCR1A = ICR1 * duty_cycle;

  //~150 microseconds

  // when you go forward, time high decreases
  // when you go backward, time high increases
}
