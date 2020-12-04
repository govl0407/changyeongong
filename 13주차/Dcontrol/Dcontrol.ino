#include <Servo.h>
Servo myservo;
// Arduino pin assignment
#define PIN_IR A0
#define PIN_LED 9
#define PIN_SERVO 10

unsigned long last_sampling_time_dist, last_sampling_time_servo, last_sampling_time_serial; // unit: ms
bool event_dist, event_servo, event_serial;
float raw_dist;

float dist_target; // location to send the ball
//===================================================
// 코드를 작동시키기 전에 _DUTY_NEU의 값을 각자의 중립위치각도로 수정 후 사용!!!
#define _DUTY_NEU 1400 // neutral position
//===================================================
#define _INTERVAL_DIST 30   // USS interval (unit: ms)
#define _INTERVAL_SERVO 20 // [3401] 서보를 20ms마다 조작하기
#define _INTERVAL_SERIAL 30 // serial interval (unit: ms)
#define _DIST_TARGET 200  //[3166]목표로 하는 탁구공 중심 위치까지 거리255mm로 고정
#define _DIST_MIN 10                       //[3164] 최소 측정 거리 10mm로 고정 
#define _DIST_MAX 410   // [3401] 측정 거리의 최댓값를 410mm로 설정
#define _DUTY_MIN 1000    //[3148]  서보의 가동 최소 각도(0)
#define _DUTY_MAX 1700
#define DELAY_MICROS  1500
#define EMA_ALPHA 0.35
#define _SERVO_ANGLE 30   //[3159] 서보의 각도(30º) 
//[3150] 레일플레이트가 사용자가 원하는 가동범위를 움직일때, 이를 움직이게 하는 서보모터의 가동범위
#define _SERVO_SPEED 800//150             //[3147]  서보 속도를 30으로 설정
#define KP 2.5
#define KD 105//22.0// 110 = 오버, 100 = 오버 or 크리티컬 70 = 언더 // [3158] 비례상수 설정
#define INTERVAL 10.0
float filtered_dist, filtered_cali_dist;
float ema_dist = 0;
float samples_num = 3;
int duty_chg_per_interval;    //[3167] 주기동안 duty 변화량 변수
int duty_target, duty_curr;    // [3157] 서보의 목표위치와 서보에 실제로 입력할 위치
int a, b;
float error_curr, error_prev, control, pterm, dterm, iterm;

float ir_distance(void) { // return value unit: mm
  float val;
  float volt = float(analogRead(PIN_IR));
  val = ((6762.0 / (volt - 9.0)) - 4.0) * 10.0;
  return val;
}

// ================
float under_noise_filter(void) {
  int currReading;
  int largestReading = 0;
  for (int i = 0; i < samples_num; i++) {
    currReading = ir_distance();
    if (currReading > largestReading) {
      largestReading = currReading;
    }
    // Delay a short time before taking another reading
    delayMicroseconds(DELAY_MICROS);
  }
  return largestReading;
}
float filtered_ir_distance(void) {
  int currReading;
  int lowestReading = 1024;
  for (int i = 0; i < samples_num; i++) {
    currReading = under_noise_filter();
    if (currReading < lowestReading) {
      lowestReading = currReading;
    }
  }
  // eam 필터 추가
  ema_dist = EMA_ALPHA * lowestReading + (1 - EMA_ALPHA) * ema_dist;
  return ema_dist;
}
//===================================================

void setup() {
  Serial.begin(57600);
  pinMode(PIN_LED, OUTPUT);
  myservo.attach(PIN_SERVO);
  a = 90;
  b = 488;
  myservo.writeMicroseconds(_DUTY_NEU);
  delay(1000);
  // initialize last sampling time
  last_sampling_time_dist= last_sampling_time_servo = last_sampling_time_serial = 0;
  event_dist = event_servo= event_serial = false;
  dist_target = _DIST_TARGET;
  duty_chg_per_interval = int((_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED /  _SERVO_ANGLE) * (float(INTERVAL) / 1000.0));
}

void loop() {
  //myservo.writeMicroseconds(_DUTY_NEU);
  unsigned long time_curr = millis();
  if (time_curr >= last_sampling_time_dist + _INTERVAL_DIST) {
    last_sampling_time_dist += _INTERVAL_DIST;
    event_dist = true;
  }
  if(time_curr >= last_sampling_time_servo + _INTERVAL_SERVO) {
        last_sampling_time_servo += _INTERVAL_SERVO;
        event_servo = true;
  }
  if (time_curr >= last_sampling_time_serial + _INTERVAL_SERIAL) {
    last_sampling_time_serial += _INTERVAL_SERIAL;
    event_serial = true;
  }
  if (event_servo) {
    event_servo = false; // [3153] servo EventHandler Ticket -> false
    // adjust duty_curr toward duty_target by duty_chg_per_interval
    if (duty_target > duty_curr) {
      duty_curr += duty_chg_per_interval;
      if (duty_curr > duty_target) duty_curr = duty_target;
    }
    else {
      duty_curr -= duty_chg_per_interval;
      if (duty_curr < duty_target) duty_curr = duty_target;
    }       //[3166] 서보가 현재위치에서 목표위치에 도달할 때까지 duty_chg_per_interval값 마다 움직임(duty_curr에 duty_chg_per_interval값 더하고 빼줌)

    // update servo position
    myservo.writeMicroseconds(duty_curr);   //[3166]위에서 바뀐 현재위치 값을 갱신
  }


  if (event_dist) {
    event_dist = false;
    raw_dist = ir_distance();
    //==============================================
    filtered_dist = filtered_ir_distance();
    filtered_cali_dist = 100 + 300.0 / (520 - 90) * (filtered_dist - 90);
    error_curr = _DIST_TARGET - filtered_cali_dist; // [3158] 목표값 에서 현재값을 뺀 값이 오차값
    pterm = KP*error_curr; 
    
    //control = pterm;           // [3158] P제어 이기때문에 pterm만 있음
    //===============================================
    dterm = KD * (error_curr - error_prev); // 미분제어
    control = dterm + pterm;
    duty_target = _DUTY_NEU + control;
    // Limit duty_target within the range of [_DUTY_MIN, _DUTY_MAX]
    if(duty_target < _DUTY_MIN) duty_target = _DUTY_MIN; // lower limit
    if(duty_target > _DUTY_MAX) duty_target = _DUTY_MAX; // upper limit
    // update error_prev
    error_prev = error_curr;

  }
  if (event_serial) {
    event_serial = false;
    // output the read value to the serial port
    Serial.print("dist_ir:");
    Serial.print(filtered_dist);
    Serial.print(",pterm:");
    Serial.print(map(pterm,-1000,1000,510,610));
    Serial.print(",dterm:");
    Serial.print(map(dterm,-1000,1000,510,610));
    Serial.print(",duty_target:");
    Serial.print(map(duty_target,1000,2000,410,510));
    Serial.print(",duty_curr:");
    Serial.print(map(duty_curr,1000,2000,410,510));
    Serial.println(",Min:100,Low:200,dist_target:255,High:310,Max:410");

    
  }
}
