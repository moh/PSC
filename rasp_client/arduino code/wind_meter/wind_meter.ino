int infra_pin = 7;
long nb_rotation = 0;
unsigned long t0, t1;
unsigned long duration;
// if the IR sensor has just read a value
bool already_read = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(infra_pin, INPUT);
  t0 = millis();
}

void loop() {
  int data = digitalRead(infra_pin);
  // 0 when it sees something
  if ((data == 0) && (!already_read)){
    nb_rotation += 1;
    t1 = millis();
    duration = t1 - t0;
    t0 = t1;
    // vitesse instantane
    Serial.print(1000/(2*duration));
    Serial.print(",");
    Serial.println(1000*nb_rotation/(2*t1));
    already_read = true;
    delay(10);
  }
  else if (data == 1 && already_read){
    already_read = false;
    //Serial.println("Not reading ! ");
    delay(10);
  }
}
