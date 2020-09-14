#define PIN_LED 7
unsigned int count, toggle;
void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  while (!Serial){
    ;
  }

  count = toggle = 1;
  digitalWrite(PIN_LED, toggle);
}

void loop() {
  Serial.println(++count);
  toggle = toggle_state(toggle);
  Serial.println(toggle);
  digitalWrite(PIN_LED, toggle);
  delay(100);
}
int toggle_state(int toggle){
  if(count <10){
    toggle = 0;
    }
  else if(count>20){
    toggle = 1;
  }
  else{
    if (count % 2 ==0){
      toggle = 0;
    }
    else{
      toggle = 1;
    }
  }
  return toggle;
}
