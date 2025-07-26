#include <Ultrasonic.h>
// Running on an Arduino Nano

#define RELAY_SWITCH_PIN 2
#define ULTRASONIC_ECHO_PIN 10
#define ULTRASONIC_TRIGGER_PIN 11

#define STOP_DISTANCE 50 // cm

#define RECEIVER_CH1_INPUT 3 // motor left right
#define RECEIVER_CH2_INPUT 5 // motor forward backward

#define RECEIVER_CH1_OUTPUT 9
#define RECEIVER_CH2_OUTPUT 6

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

void config_timer() { 
  //https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf pg 33
  CLKPR = _BV(CLKPCE);
  CLKPR = _BV(CLKPS1)| _BV(CLKPS0); //Clock Prescaler

  // TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
  // //7, 5, 1, 0
  // TCCR1B = _BV(CS12);
  // OCR1A = 180;
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

void connect_receiver(int input_ch, int output_ch){
  float t_high = pulseIn(input_ch, HIGH);
  float t_low = pulseIn(input_ch, LOW);
  float duty_cycle = (t_high / (t_high + t_low)) * 100;
  Serial.print("T-high: " + String(t_high) + " "); // Regular t-h is 1500 ms i think (it extends to 2000 and retracts to 1000)
  Serial.print("Duty Cycle is: " + String(duty_cycle));
  // OCR1A = ICR1 * duty_cycle; //sets the duty cycle for PIN 9 SPECIFICALLY

  float new_signal = map(duty_cycle, 0, 100, 0, 255);
  analogWrite(output_ch, new_signal); //From 0 to 255 representing the duty cycle

  //~150 microseconds

  // when you go forward, time high decreases
  // when you go backward, time high increases
}

void loop(){
  connect_receiver(RECEIVER_CH1_INPUT, RECEIVER_CH1_OUTPUT); // reads the signal in, and reconstructs with the same duty cycle
  // the ch1 output is motor forward and backward, (the mdds will convert the wave into directions for the motor) we don't need t
  

  int dist = ultrasonic.read(CM); // TODO what to do when getting a whack value, add a filter to track the change in values, and have it in a reasonable range
  Serial.println(dist);
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
