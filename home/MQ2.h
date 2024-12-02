class MQ2{
  protected:
  int MQ2_pin;
  int MQ2Value;
  public:
  void setupMQ2();
  void ReadSensor();

  int getMQ2Value(){
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
    Serial.printf("MQ2 Value: %d\n", MQ2Value);
    lastReadTime = now;
  }
}
