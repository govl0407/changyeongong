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
#define LENGTH 30
#define k_LENGTH 8
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
float alpha = 0.1;
float dist_ema = 0.0;
int cnt;
float sum;
float dist_list[LENGTH];

void setup() {
// initialize GPIO pins
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(1700);
  a = 79;
  b = 350;
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
float filter(int d){
  float filtered; 
  cnt = 0;
  sum = 0;
  while (cnt < LENGTH) //LENGTH 값 개수
  {
    dist_list[cnt] = 100 + 300.0 / (b - a) * (d - a);
    sum = dist_list[cnt];
    cnt++;
  }
  
  
  // 선택 정렬
  for (int i = 0; i < LENGTH-1; i++){
    for (int j = i+1; j < LENGTH; j++){
      if (dist_list[i] > dist_list[j]) {
        float tmp = dist_list[i];
        dist_list[i] = dist_list[j];
        dist_list[j] = tmp;
      }
    }
  }
  
  // dist_cali값 계산 과정
  for (int i = 0; i < k_LENGTH; i++) {
    sum -= dist_list[i];
  }
  for (int i = 1; i <= k_LENGTH; i++) {
    sum -= dist_list[LENGTH-i];
  }

  filtered = sum/(LENGTH-k_LENGTH);
  
  // ema filter 적용
  dist_ema = alpha*filtered + (1-alpha)*dist_ema;

  return dist_ema;
}

void loop() {
  float raw_dist = ir_distance();
  float dist_cali = 100 + 300.0 / (b - a) * (raw_dist - a);
  float filtered_dist = filter(dist_cali);
  Serial.print("min:0,max:500,dist:");
  Serial.print(raw_dist);
  Serial.print(",dist_cali:");
  Serial.println(dist_cali);
  Serial.print(",filter:");
  Serial.print(filtered_dist);
  Serial.print("servo:");
  Serial.print(myservo.read());
  
  if(dist_cali <= 300 && dist_cali >=225 ) myservo.writeMicroseconds(1560);
  else if(dist_cali > 300) myservo.writeMicroseconds(1560- (dist_cali-300));
  else if(dist_cali < 225) myservo.writeMicroseconds(1560 +(225- dist_cali));
  delay(20);
  myservo.attach(PIN_SERVO); 


}
