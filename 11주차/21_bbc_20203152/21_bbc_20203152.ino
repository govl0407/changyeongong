#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10
#define PIN_IR A0
// configurable parameters
#define _DUTY_MIN 553 // servo full clockwise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counterclockwise position (180 degree)

#define _POS_START (_DUTY_MIN + 100)
#define _POS_END (_DUTY_MAX - 100)

#define _SERVO_SPEED 60 // servo speed limit (unit: degree/second)
#define INTERVAL 20  // servo update interval
float serial_start;
float serial_end;
int a, b;
// global variables
unsigned long last_sampling_time; // unit: ms
int duty_chg_per_interval; // maximum duty difference per interval
int toggle_interval, toggle_interval_cnt;
float pause_time; // unit: sec
Servo myservo;
int duty_target, duty_curr;

void setup() {
// initialize GPIO pins
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(1500);
  a = 79;
  b = 330;
// initialize serial port
  Serial.begin(57600);
  last_sampling_time = 0;
}
float ir_distance(void){ // return value unit: mm
  float val;
  float volt = float(analogRead(PIN_IR));
  val = ((6762.0/(volt-9.0))-4.0) * 10.0;
  return val;
}
void loop() {
  float raw_dist = ir_distance();
  float dist_cali = 100 + 300.0 / (b - a) * (raw_dist - a);
  Serial.print("min:0,max:500,dist:");
  Serial.print(raw_dist);
  Serial.print(",dist_cali:");
  Serial.println(dist_cali);
  if(dist_cali > 255) myservo.writeMicroseconds(1350);
  else if(dist_cali < 255) myservo.writeMicroseconds(1750);
  delay(20);
  myservo.attach(PIN_SERVO); 


}
