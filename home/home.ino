#include <Arduino.h>
#include "DHT11.h"
#include "RealTime.h"
#include "LCD1602.h"
#include "lightSensor.h"
#include "MQ2.h"
#include "emergency.h"

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
RealTime timer;
LCD1602 lcd;
Emgergency em_mode;

RTC_DATA_ATTR bool modeState = NORMAL;
RTC_DATA_ATTR bool modetmp = NORMAL;

void setup() {
  Serial.begin(115200);
  sensor.setupSensor();
  timer.setupTime();
  lcd.setupLCD();
  em_mode.setupBuzzer();
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
  // Clear LCD when mode changes
  if (modetmp != modeState) {
    lcdController.clear();
    modetmp = modeState;  // Update modetmp to reflect the new mode
    if (modeState == NORMAL) {
      startTime = millis(); // Reset start time when entering NORMAL mode
    }
  }

  // Check MQ2 value to set mode state
  if (sensor.gasSensor.getMQ2Value() > 200) {
    modeState = EMERGENCY;
  } else {
    modeState = NORMAL;
  }

  // Run different logic based on mode
  if (modeState == EMERGENCY) {
    sensor.ReadSensor();
    timer.GetTime();
    timer.PrintTime();
    em_mode.EmergencyMode();
  } else {
    lcdController.backlight();
    sensor.ReadSensor();
    timer.GetTime();
    timer.PrintTime();
    lcd.displayLCD(sensor.dht11, timer);

    // Check if it is within the time range and 20 seconds have passed since NORMAL mode
    if ((timer.GetHour() < 5 || timer.GetHour() > 15) && millis() - startTime >= 100000) {
      enterDeepSleepMode();
    }
  }
}
