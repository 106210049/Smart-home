
class LightSensor{
  protected:
  int lightSensor_pin;
  int lightValue;
  public:
  void setupLightSensor();
  void ReadSensor();
  int GetlightIntensity(){
    
    return this->lightValue;
  }
};
void LightSensor::setupLightSensor(){
  lightSensor_pin=33;
  pinMode(lightSensor_pin,INPUT);
  Serial.println("Set up light sensor oke");
}
void LightSensor::ReadSensor(){
  lightValue=digitalRead(lightSensor_pin);
  // Serial.print("light val: ");
  // Serial.println(lightValue);
  // lightValue=10;
}
