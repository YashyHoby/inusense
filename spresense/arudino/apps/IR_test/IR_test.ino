
#define IRPIN 7
int bState = 0;

void setup() {
    pinMode(LED0, OUTPUT);
    pinMode(IRPIN, INPUT);
    Serial.begin(115200);
    delay(5000); //delay(30000); // wait for 30s until the sensor voltage becomes stable
}

void loop() {
  /*
  int State = digitalRead(IRPIN);

  if(bState != State){
    bState = State;
    Serial.println("");
  }

  if(State == HIGH){
    Serial.print(State);
    digitalWrite(LED0, HIGH);
  }else{
    Serial.print(State);
    digitalWrite(LED0, LOW);
  }
  */

  int analogValue = analogRead(A0);
  //float voltage = analogValue * (5.0/1024.0);
  Serial.print(analogValue);
  Serial.println("");
  // if (voltage >= 3.0){
  //   digitalWrite(LED0, HIGH);
  // }else{
  //   digitalWrite(LED0, LOW);
  // }

  delay(200);
}