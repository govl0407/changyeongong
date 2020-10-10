#include <Servo.h>
#define PIN_SERVO 10

Servo myservo;
int serv = 0;
int de = 90;
void setup() {
  myservo.attach(PIN_SERVO); 
  Serial.begin(57600);
  myservo.write(90);
  delay(1000);
  
}

void loop() {
  
  myservo.write(serv);
  Serial.print(serv);
  Serial.print('\n');
  Serial.print(de);
  Serial.print('\n');
  delay(500);
  
  serv = serv + de;
  if (serv >=180){
    de = -de;
  }
  else if (serv<=0){
    de = -de;
  }
}
