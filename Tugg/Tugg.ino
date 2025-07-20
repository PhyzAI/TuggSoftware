#include <Ultrasonic.h>
#include <Servo.h>
//Running on an Arduino Nano


#define RELAY_SWITCH_PIN 6
#define ULTRASONIC_ECHO_PIN 10
#define ULTRASONIC_TRIGGER_PIN  11

#define STOP_DISTANCE 50 //cm

#define RECEIVER_CH1_INPUT  2 //motor left
#define RECEIVER_CH2_INPUT 3 //motor right

#define RECEIVER_CH1_OUTPUT  9
#define RECEIVER_CH2_OUTPUT  8

Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);
float prev = 0;

void setup() {
  //TODO: if the sensor is touching something else, it will output really high, incorrect numbers - may not be a problem?

  Serial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_SWITCH_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT_PULLUP); //if nothing is connected set input to high by default

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY_SWITCH_PIN, LOW);
  //We want pin 2 to be LOW regularly, and pin 2 to be HIGH if it gets too close
  //This is because a high signal will activate the relay which will keep the switches active. When a low signal it sent, the switches will release, and power will be cut to the motors
}

void connect_reciever(int input_ch, int output_ch){
  float t_high = pulseIn(input_ch, HIGH);
  float t_low = pulseIn(input_ch, LOW);
  float duty_cycle = (t_high/(t_high+t_low)) * 100;
  Serial.print("T-high: ");
  Serial.println(t_high); //Regular t-h is 1500 ms i think (it extends to 2000 and retracts to 1000)

  float new_signal = map(duty_cycle, 0, 100, 0, 255);
  analogWrite(output_ch, new_signal); //From 0 to 255 representing the duty cycle
  Serial.print("Duty Cycle is: ");
  Serial.println(duty_cycle);

  //when you go forward, time high decreases
  //when you go backward, time high increases
}


void loop() {
  connect_reciever(RECEIVER_CH1_INPUT, RECEIVER_CH1_OUTPUT); //reads the signal in, and reconstructs with the same duty cycle
  // connect_reciever(RECEIVER_CH2_INPUT, RECEIVER_CH2_OUTPUT);

  int dist = ultrasonic.read(CM);
  // Serial.println(dist);

  if (dist < STOP_DISTANCE){
    Serial.println("Too Close!!!!");
    digitalWrite(LED_BUILTIN, HIGH);    
    digitalWrite(RELAY_SWITCH_PIN, HIGH);
    //Disable forward movement
  }else{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(RELAY_SWITCH_PIN, LOW);
  }
  
}
