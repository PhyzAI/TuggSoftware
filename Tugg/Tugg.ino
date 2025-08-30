#include <Ultrasonic.h>
// Running on an Arduino Nano

#define RELAY_SWITCH_PIN 2         // motor power cutoff relay control
#define ULTRASONIC_ECHO_PIN 10     // ultrasonic rx   
#define ULTRASONIC_TRIGGER_PIN 11  // ultrasonic tx 
#define RECEIVER_CH1_INPUT 3       // motor left right
#define RECEIVER_CH2_INPUT 5       // motor forward backward
#define RECEIVER_CH6_INPUT 6       // set distance input
int us_dist =0;

Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

void config_pins()
{
  pinMode(RECEIVER_CH1_INPUT, INPUT_PULLUP);  // Motor F/B input
  pinMode(RECEIVER_CH2_INPUT, INPUT_PULLUP);  // Motor L/R input
  pinMode(RECEIVER_CH6_INPUT, INPUT_PULLUP);  // Distance Sensitivity input
  pinMode(LED_BUILTIN, OUTPUT);               // Status LED
  pinMode(RELAY_SWITCH_PIN, OUTPUT);          // Motor cutoff to relay
  pinMode(ULTRASONIC_ECHO_PIN, INPUT_PULLUP); // if nothing is connected set input to high by default
}

void setup(){
  // TODO: if the sensor is touching something else, it will output really high, incorrect numbers
  Serial.begin(9600);    // turn on serial port for debug only
  config_pins();
  digitalWrite(LED_BUILTIN, LOW);         // start with the LED alarm low and the relay engaged
  digitalWrite(RELAY_SWITCH_PIN, LOW);
  // We want pin 2 to be LOW regularly, and pin 2 to be HIGH if it gets too close
  // This is because a high signal will activate the relay which will keep the switches active. When a low signal it sent, the switches will release, and power will be cut to the motors
}

void connect_receiver(int input_ch, int* pwm_value){
  float t_high = pulseIn(input_ch, HIGH);
  float t_low  = pulseIn(input_ch, LOW);
  float duty_cycle = (t_high / (t_high + t_low)) * 100;

  *pwm_value = map(duty_cycle, 0, 20, 0, 255);
  if(*pwm_value >255) { 
    *pwm_value=255;
  }
}

void loop(){
int stop_distance; // initial stop_distance 
int f_b_value    ;  // initial forward/backward
int l_r_value    ;  // initial laft/right
  connect_receiver(RECEIVER_CH1_INPUT, &f_b_value); // reads the signal in, and reconstructs with the same duty cycle
  connect_receiver(RECEIVER_CH2_INPUT, &l_r_value);
  connect_receiver(RECEIVER_CH6_INPUT, &stop_distance);
 
  int us_dist_last = us_dist;
  int us_dist = map(ultrasonic.read(CM), 0, 260, 50, 140); 
  if (us_dist > 140) { //Do not allow out-of-range values
    us_dist=140;
  }
  /*         IMPROVE THIS FILTER
  int filter_thresh = abs(us_dist-us_dist_last);
  if ( filter_thresh > 50) {
    us_dist = us_dist_last; // I think this will NOT work
    Serial.println("Failed Filter threshold!");
  }
  */
  char printvar[100];
  sprintf(printvar,"StopDistance: %03d, US_Dist: %03d, F/B: %03d \n", stop_distance, us_dist, f_b_value );
  Serial.print(printvar);
  
  if (f_b_value < 100){
    if (us_dist < stop_distance){
      Serial.println("Too Close, going forward");
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(RELAY_SWITCH_PIN, HIGH);
    }
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(RELAY_SWITCH_PIN, LOW);
  }
}
