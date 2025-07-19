// Example skeleton code For Tugg: The middle Pulse width needs to be adjusted by increasing the deadzone or calibrating to the actual motor driver that we use. The Matrix that control's which direction gets disabled isn't fully implemented, but is in a working state.
// Disabled Movement directions is controlled by:  "if (ch1_motor1_raw >= MAX_PW_US || ch1_motor1_raw <= MIN_PW_US) ch1_motor1_dir *= -1;", where -1 is the direction and magnitude of the motor. the motor goes from -1 to 1 (1000ms to 2000ms). 1500us is about 0 movement, which is set to the default.
// Validated functions/features: RC Control signal pass through, Ultrasonic sonic disable, debug mode.
// 20250719, KI with the help with ChatGpt.

#include <Servo.h>

// ============================================================================
//                          CONFIGURATION MACROS
// ============================================================================

// Enables debug mode to simulate dummy RC signals (set to 0 for real PWM input)
#define DEBUG_MODE                0

// Enables ultrasonic obstacle avoidance logic
#define ENABLE_ULTRASONIC_BLOCK  1

// Input Pins (for RC PWM signals)
#define CH1_MOTOR1_IN_PIN         2    // Must support external interrupt (INT0)
#define CH2_MOTOR2_IN_PIN         3    // Must support external interrupt (INT1)

// Output Pins (Servo or ESC control)
#define CH1_MOTOR1_OUT_PIN        9
#define CH2_MOTOR2_OUT_PIN        10

// Ultrasonic Sensor Pins
#define ULTRASONIC_TRIG_PIN       A0
#define ULTRASONIC_ECHO_PIN       A1

// Default and Range Values for RC PWM (microseconds)
#define DEFAULT_PW_US             1500
#define MIN_PW_US                 1000
#define MAX_PW_US                 2000
#define DEADZONE_US               25      // Deadzone around center

// Timing Parameters
#define SERVO_UPDATE_US           20000   // Servo update every 20ms
#define INPUT_TIMEOUT_US          100000  // 100ms timeout = signal lost

// Smoothing (Exponential Moving Average)
#define SMOOTHING_ALPHA           0.2     // 0 = no smoothing, 1 = full smoothing

// Obstacle Avoidance Threshold (cm)
#define BLOCK_DISTANCE_CM         20

// ============================================================================
//                          GLOBAL VARIABLES
// ============================================================================

#if !DEBUG_MODE
volatile unsigned long ch1_motor1_start_time = 0;
volatile unsigned long ch2_motor2_start_time = 0;
volatile int ch1_motor1_raw = DEFAULT_PW_US;
volatile int ch2_motor2_raw = DEFAULT_PW_US;
volatile unsigned long ch1_last_update = 0;
volatile unsigned long ch2_last_update = 0;
#else
int ch1_motor1_raw = DEFAULT_PW_US;
int ch2_motor2_raw = DEFAULT_PW_US;
int ch1_motor1_dir = 1;
int ch2_motor2_dir = -1;
#endif

int ch1_motor1_filtered = DEFAULT_PW_US;
int ch2_motor2_filtered = DEFAULT_PW_US;

unsigned long last_servo_update = 0;

Servo motor1_servo;
Servo motor2_servo;

// ============================================================================
//                          INTERRUPT HANDLERS
// ============================================================================

#if !DEBUG_MODE
void ISR_ch1_motor1() {
  if (digitalRead(CH1_MOTOR1_IN_PIN)) {
    ch1_motor1_start_time = micros();
  } else {
    ch1_motor1_raw = micros() - ch1_motor1_start_time;
    ch1_last_update = micros();
  }
}

void ISR_ch2_motor2() {
  if (digitalRead(CH2_MOTOR2_IN_PIN)) {
    ch2_motor2_start_time = micros();
  } else {
    ch2_motor2_raw = micros() - ch2_motor2_start_time;
    ch2_last_update = micros();
  }
}
#endif

// ============================================================================
//                          SETUP FUNCTION
// ============================================================================

void setup() {
#if !DEBUG_MODE
  pinMode(CH1_MOTOR1_IN_PIN, INPUT);
  pinMode(CH2_MOTOR2_IN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(CH1_MOTOR1_IN_PIN), ISR_ch1_motor1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH2_MOTOR2_IN_PIN), ISR_ch2_motor2, CHANGE);
#endif

  motor1_servo.attach(CH1_MOTOR1_OUT_PIN);
  motor2_servo.attach(CH2_MOTOR2_OUT_PIN);

#if ENABLE_ULTRASONIC_BLOCK
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
#endif
}

// ============================================================================
//                          ULTRASONIC SENSOR FUNCTION
// ============================================================================

// Measures distance using HC-SR04 (non-blocking as much as possible)
long readUltrasonicDistance() {
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 25000); // timeout ~25ms
  long distance_cm = duration / 58;
  return distance_cm;
}

// ============================================================================
//                          OBSTACLE AVOIDANCE FUNCTION
// ============================================================================

void applyObstacleAvoidance(float &m1, float &m2) {
#if ENABLE_ULTRASONIC_BLOCK
  long dist_cm = readUltrasonicDistance();

  // If object is too close, block forward motion only
  if (dist_cm > 0 && dist_cm <= BLOCK_DISTANCE_CM) {
    if (m1 > 0) m1 = 0; // Disable forward
    if (m2 > 0) m2 = 0;
  }
#endif
}

// ============================================================================
//                          MAIN LOOP
// ============================================================================

void loop() {
  unsigned long now = micros();

  // Update simulated PWM if in debug mode
#if DEBUG_MODE
  ch1_motor1_raw += ch1_motor1_dir * 10;
  if (ch1_motor1_raw >= MAX_PW_US || ch1_motor1_raw <= MIN_PW_US) ch1_motor1_dir *= -1;

  ch2_motor2_raw += ch2_motor2_dir * 15;
  if (ch2_motor2_raw >= MAX_PW_US || ch2_motor2_raw <= MIN_PW_US) ch2_motor2_dir *= -1;
#endif

  // Timeout failsafe: revert to neutral if no recent signal
#if !DEBUG_MODE
  if (now - ch1_last_update > INPUT_TIMEOUT_US) ch1_motor1_raw = DEFAULT_PW_US;
  if (now - ch2_last_update > INPUT_TIMEOUT_US) ch2_motor2_raw = DEFAULT_PW_US;
#endif

  // Apply exponential moving average smoothing
  ch1_motor1_filtered = ch1_motor1_filtered * (1.0 - SMOOTHING_ALPHA) + ch1_motor1_raw * SMOOTHING_ALPHA;
  ch2_motor2_filtered = ch2_motor2_filtered * (1.0 - SMOOTHING_ALPHA) + ch2_motor2_raw * SMOOTHING_ALPHA;

  // Apply deadzone
  if (abs(ch1_motor1_filtered - DEFAULT_PW_US) < DEADZONE_US) ch1_motor1_filtered = DEFAULT_PW_US;
  if (abs(ch2_motor2_filtered - DEFAULT_PW_US) < DEADZONE_US) ch2_motor2_filtered = DEFAULT_PW_US;

  // Update motors at fixed interval (non-blocking)
  if (now - last_servo_update >= SERVO_UPDATE_US) {
    last_servo_update = now;

    // Convert to normalized value (-1 to 1), where 0 is neutral
    float m1_norm = (ch1_motor1_filtered - DEFAULT_PW_US) / 500.0;
    float m2_norm = (ch2_motor2_filtered - DEFAULT_PW_US) / 500.0;

    // Apply obstacle logic (disables forward)
    applyObstacleAvoidance(m1_norm, m2_norm);

    // Convert normalized back to pulse width
    int m1_output = DEFAULT_PW_US + (int)(m1_norm * 500.0);
    int m2_output = DEFAULT_PW_US + (int)(m2_norm * 500.0);

    // Constrain outputs
    m1_output = constrain(m1_output, MIN_PW_US, MAX_PW_US);
    m2_output = constrain(m2_output, MIN_PW_US, MAX_PW_US);

    // Output to motors
    motor1_servo.writeMicroseconds(m1_output);
    motor2_servo.writeMicroseconds(m2_output);
  }
}
