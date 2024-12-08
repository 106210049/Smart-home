#include <Arduino.h>
#include <WiFi.h>
#include "DHT11.h"       // DHT11 sensor lib
#include "lightSensor.h" // Light sensor lib
#include "MQ2.h"         // Gas sensor lib
#include "LCD1602.h"     // LCD16x02 lib
#include "emergency.h"   // Emergency mode lib
#include "motor.h"       // Motor driver lib
#include "ESP_Rainmaker.h"
// #include "Blynk.h"
#define EMERGENCY 1
#define NORMAL 0

const int sleepDuration = 5000000; // Set sleep duration in microseconds (e.g.,10s)
unsigned long currentTime = millis();
unsigned long startTime = 0; // Variable to track the start time of NORMAL mode

class Sensor : public DHTSensor, public LightSensor, public MQ2 {
public:
  DHTSensor dht11;
  LightSensor lightsensor;
  MQ2 gasSensor;
  void setupSensor();
  void ReadSensor();
};

void Sensor::setupSensor() {
  dht11.setupDHT11();
  lightsensor.setupLightSensor();
  gasSensor.setupMQ2();
}

void Sensor::ReadSensor() {
  dht11.ReadSensor();
  lightsensor.ReadSensor();
  gasSensor.ReadSensor();
}

Sensor sensor;
LCD1602 lcd;
Emgergency em_mode;
Motor motorController;

RTC_DATA_ATTR bool modeState = NORMAL;
RTC_DATA_ATTR bool modetmp = NORMAL;


void setup() {
  Serial.begin(115200);
  sensor.setupSensor();
  RainmakerSetup();
  lcd.setupLCD();
  em_mode.setupBuzzer();
  motorController.setupMotor();
  esp_sleep_enable_timer_wakeup(sleepDuration);
}

void enterDeepSleepMode() {
  Serial.println("Entering deep sleep...");
  lcdController.clear();
  lcdController.noBacklight();
  delay(10);
  esp_deep_sleep_start();
}

void loop() {

  sensor.ReadSensor(); // Đọc tất cả cảm biến trước
  
  RainMakerActive(sensor.dht11.getTemperature(),sensor.dht11.getHumidity(),sensor.lightsensor.GetlightIntensity(),sensor.gasSensor.getMQ2Value());
  if (modetmp != modeState) {
    lcdController.clear();
    modetmp = modeState;  // Update modetmp to reflect the new mode
    if (modeState == NORMAL) {
      startTime = millis(); // Reset start time when entering NORMAL mode
    }
  }
  if (sensor.gasSensor.getMQ2Value() > 50) {
    modeState = EMERGENCY;
  } else {
    modeState = NORMAL;
  }

  if (modeState == EMERGENCY) {
    motorController.MotorOn();
    em_mode.EmergencyMode();
  } else {
    motorController.MotorOff();
    lcdController.backlight();
    lcd.displayLCD(sensor.dht11);
    
  }
}