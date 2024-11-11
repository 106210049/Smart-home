
#include <WiFi.h>
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;  // GMT+7, đổi sang giây
const int   daylightOffset_sec = 3600;

class WiFiController{
  protected:
  const char* ssid;
  const char* password;
  public:
  friend class RealTime;
  void connectWifi();
  void disconnectWifi();
};
void WiFiController::connectWifi(){
  // Connect to Wi-Fi
  ssid="Son";
  password = "77687768";

  Serial.begin(115200);

  // Kết nối Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

}
void WiFiController::disconnectWifi(){
  // Ngắt kết nối Wi-Fi vì không cần nữa
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
WiFiController wifiController;

class RealTime{
  protected:
  
  int hour, minute, second, day, month, year;
  char weekDay[10];
  char monthName[10];
  public:
  friend class LCD1602;
  void setupTime();
  void GetTime();
  void PrintTime();
  int GetHour(){
    return hour;
  }
};
void RealTime::setupTime(){
  wifiController.connectWifi();
  // Cấu hình và lấy thời gian từ NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  GetTime();
  Serial.println("Set up time OK");
  wifiController.disconnectWifi();
}
void RealTime::GetTime(){
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Gán các giá trị thời gian vào biến
  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;
  second = timeinfo.tm_sec;
  day = timeinfo.tm_mday;
  month = timeinfo.tm_mon + 1;  // tm_mon tính từ 0
  year = timeinfo.tm_year + 1900;  // tm_year tính từ 1900

  // Lưu trữ ngày và tháng dưới dạng chuỗi
  strftime(weekDay, sizeof(weekDay), "%A", &timeinfo);
  strftime(monthName, sizeof(monthName), "%B", &timeinfo);
}
void RealTime::PrintTime(){
  Serial.println("Stored Time:");
  Serial.printf("Date: %s, %s %d, %d\n", weekDay, monthName, day, year);
  Serial.printf("Time: %02d:%02d:%02d\n", hour, minute, second);
  Serial.println();
}
