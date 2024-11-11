#ifndef LCD1602_H
#define LCD1602_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcdController(0x27, 16, 2);  // Địa chỉ I2C của LCD có thể là 0x3F hoặc 0x27

unsigned long previousMillis = 0;
const long interval = 30000;  // 30 giây
RTC_DATA_ATTR bool showTime = true;         // Bắt đầu bằng việc hiển thị thời gian

class LCD1602{
  public:
  void setupLCD();
  void displayTimeOnLCD(RealTime& timer);
  void displayTemperatureAndHumidityOnLCD(DHTSensor& dht11);
  void displayLCD(DHTSensor& dht11, RealTime& timer);
};
void LCD1602::setupLCD(){
  Wire.begin(21,22);
  lcdController.init();         // Initialize LCD
  lcdController.backlight();    // Turn on the backlight
  lcdController.clear();        // Clear any previous data
  Serial.println("LCD Set up OK");
}

 void LCD1602::displayTimeOnLCD(RealTime& timer) {
        lcdController.setCursor(0, 0);
        lcdController.printf("Date: %02d/%02d/%d", timer.day, timer.month, timer.year);
        lcdController.setCursor(0, 1);
        lcdController.printf("Time: %02d:%02d:%02d", timer.hour, timer.minute, timer.second);
//        Serial.println(timer.day);
//        Serial.println(timer.month);
//        Serial.println(timer.year);
    }
//
//
void LCD1602::displayTemperatureAndHumidityOnLCD(DHTSensor& dht11) {
        if (dht11.temp=="NaN"||dht11.hum=="NaN") {
            lcdController.setCursor(0, 0);
            lcdController.print("Error reading");
            lcdController.setCursor(0, 1);
            lcdController.print("sensor data");
            return;
        }
          lcdController.setCursor(0, 0);
          lcdController.print("Temp: " + dht11.temp + " oC");
          lcdController.setCursor(0, 1);
          lcdController.print("Hum: " + dht11.hum + " %");
    }

void LCD1602::displayLCD(DHTSensor& dht11, RealTime& timer){
  unsigned long currentMillis = millis();

  // Cập nhật nội dung hiển thị mỗi 30 giây
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    showTime = !showTime;  // Đổi trạng thái hiển thị giữa thời gian và nhiệt độ/độ ẩm
    lcdController.clear();
  }

  if (showTime) {
    displayTimeOnLCD(timer);
  } else {
    displayTemperatureAndHumidityOnLCD(dht11);
  }
}
#endif
