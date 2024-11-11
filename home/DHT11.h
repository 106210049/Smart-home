#ifndef DHT11_H
#define DHT11_H
#include "DHTesp.h"
class DHTSensor{
  protected:
  int DHT_pin;
  String temp,hum;
  DHTesp dhtSensor;
  public:
  friend class LCD1602;
  void setupDHT11();
  void ReadSensor();
//  String getTemperature() const { return temp; }
//  String getHumidity() const { return hum; }
};
void DHTSensor::setupDHT11(){
  DHT_pin=18;
  dhtSensor.setup(DHT_pin, DHTesp::DHT22);
  Serial.println("Set up DHT Sensor Successfull");
}
void DHTSensor::ReadSensor(){
  TempAndHumidity data =dhtSensor.getTempAndHumidity();
  temp = String(data.temperature, 1);
  hum = String(data.humidity, 1);
  if (!isnan(data.temperature) && !isnan(data.humidity))
  {
    temp = String(data.temperature, 1);
    hum = String(data.humidity, 1);
  }
  else
  {
    temp = "NaN";
    hum = "NaN";
  }
//  Serial.println(temp);
//  Serial.println(hum);
}
#endif
