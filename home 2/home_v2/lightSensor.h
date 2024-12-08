
class LightSensor{
  protected:
  int lightSensor_pin;
  float lightValue;
  public:
  void setupLightSensor();
  void ReadSensor();
  float GetlightIntensity(){
    
    return this->lightValue;
  }
};
void LightSensor::setupLightSensor(){
  lightSensor_pin=33;
  pinMode(lightSensor_pin,INPUT);
  Serial.println("Set up light sensor oke");
}
void LightSensor::ReadSensor(){
  lightValue=analogRead(lightSensor_pin);
  lightValue=map(lightValue,0, 4095, 4000, 0);
}
