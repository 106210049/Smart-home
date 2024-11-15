#include <esp_now.h>
#include <WiFi.h>
#include "setTime.h"  // Import tệp setTime.h

// Địa chỉ MAC của thiết bị nhận - thay đổi theo địa chỉ MAC của thiết bị đích
// uint8_t broadcastAddress[] = {0x84, 0xFC, 0xE6, 0x00, 0xE7, 0xC0};  // ESP C3
uint8_t broadcastAddress[] = {0x34, 0x98, 0x7A, 0xBA, 0xB6, 0x74};  // ESP CAM

// Callback khi dữ liệu được gửi
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback khi dữ liệu được nhận
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  TimeRequest receivedTiming;
  memcpy(&receivedTiming, incomingData, sizeof(receivedTiming));

  Serial.print("Received timeOn: ");
  Serial.println(receivedTiming.timeOn);
  Serial.print("Received timeOff: ");
  Serial.println(receivedTiming.timeOff);
}

void setup() {
   Serial.begin(115200);

  // Đặt thiết bị ở chế độ Wi-Fi Station
  WiFi.mode(WIFI_STA);
  delay(1000);  // Thêm thời gian chờ
  Serial.println("WiFi mode set to WIFI_STA");

  // Khởi tạo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("ESP-NOW initialized successfully");
  }

  // Đăng ký Callback để kiểm tra trạng thái gửi
  esp_now_register_send_cb(OnDataSent);

  // Thêm thông tin peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Thêm peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
} else {
    Serial.println("Peer added successfully");
}


  // Đăng ký Callback để xử lý dữ liệu nhận
  esp_now_register_recv_cb(OnDataRecv);
}

void sendMessage(TimeRequest timing) {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &timing, sizeof(timing));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.print("Error sending the data, code: ");
    Serial.println(result);  // In mã lỗi
  }
}

void loop() {
  // Sử dụng hàm createTimeRequest từ setTime.h để tạo dữ liệu TimeRequest
  TimeRequest timeRequest = createTimeRequest(2000, 5000);

  // Gửi dữ liệu `timeRequest`
  sendMessage(timeRequest);

  delay(5000);  // Gửi lại sau mỗi 5 giây
}
