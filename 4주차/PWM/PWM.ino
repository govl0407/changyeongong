// LED breathing using analog input (PWM)
#include <math.h>

void setup()
{
  pinMode(7, OUTPUT); // analog output
}

void loop()
{
  int period= 1000;
  int duty = 100;
  for(int i=0; i<=255*(duty/100); i++){
    analogWrite(7, i);
    delayMicroseconds(period);
  }
  for(int i=255*(duty/100); i>=0; i--){
    analogWrite(7, i);
    delayMicroseconds(period);
  }
}
