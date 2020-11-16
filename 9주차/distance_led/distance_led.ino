// Arduino pin assignment
#define PIN_IR A0
#define PIN_LED 9
#define dist_min 90
#define dist_max 370
void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED, 1);
// initialize serial port
  Serial.begin(57600);

}

float ir_distance(void){ // return value unit: mm
  float val;
  float volt = float(analogRead(PIN_IR));
  val = ((6762.0/(volt-9.0))-4.0) * 10.0;
  return val;
}

void loop() {
  float raw_dist = ir_distance();
  Serial.print("min:0,max:500,dist:");
  Serial.println(raw_dist);
  if(raw_dist>dist_max || raw_dist>dist_min){
    analogWrite(9, 255);
  }
  else{
    analogWrite(9, 0);
  }
  delay(20);
}
