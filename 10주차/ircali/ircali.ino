// ema + lowpass https://blog.naver.com/dongjungim20/221353270784
#include <Servo.h>
#define PIN_SERVO 10
// Arduino pin assignment
#define PIN_IR A0
#define PIN_LED 9
#define SIZE 10
float sum;
float EMA_a = 0.4;
int EMA_S_pre=0;
int EMA_S=0;
int buffer[SIZE];
float volts;
float filtered_dist;
float Filtered;

int a, b; // unit: mm
Servo myservo;
void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED, 1);
// initialize serial port
  Serial.begin(57600);
  a = 78;
  b = 350;
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


  if(raw_dist > 156 && raw_dist <224) digitalWrite(PIN_LED, 0);
  else digitalWrite(PIN_LED, 255);
  delay(20);
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(1360);


}
