class MQ2{
  protected:
  int MQ2_pin;
  float MQ2Value;
  public:
  void setupMQ2();
  void ReadSensor();

  float getMQ2Value(){
    return this->MQ2Value;
  }
};
void MQ2::setupMQ2(){
  MQ2_pin=32;
  pinMode(MQ2_pin,INPUT);
  Serial.println("Set up gas sensor oke");
}
void MQ2::ReadSensor(){
  static unsigned long lastReadTime = 0;
  unsigned long now = millis();
  if (now - lastReadTime > 500) { // Đọc mỗi 500ms
    MQ2Value = analogRead(MQ2_pin);
    MQ2Value = map(MQ2Value, 0, 4095, 0, 100);
    // Serial.printf("MQ2 Value: %d\n", MQ2Value);
    lastReadTime = now;
  }
}
