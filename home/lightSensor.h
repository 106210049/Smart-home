
class LightSensor{
  protected:
  int lightSensor_pin;
  int lightValue;
  public:
  void setupLightSensor();
  void ReadSensor();
};
void LightSensor::setupLightSensor(){
  lightSensor_pin=5;
  pinMode(lightSensor_pin,INPUT);
}
void LightSensor::ReadSensor(){
  lightValue=analogRead(lightSensor_pin);
}
