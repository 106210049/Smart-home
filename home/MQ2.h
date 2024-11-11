class MQ2{
  protected:
  int MQ2_pin;
  int MQ2Value;
  public:
  void setupMQ2();
  void ReadSensor();
  int getMQ2Value(){
    Serial.println(MQ2Value);
    return MQ2Value;
  }
};
void MQ2::setupMQ2(){
  MQ2_pin=25;
  pinMode(MQ2_pin,INPUT);
}
void MQ2::ReadSensor(){
  MQ2Value=analogRead(MQ2_pin);
  
}
