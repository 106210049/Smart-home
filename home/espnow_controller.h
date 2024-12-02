#include <esp_now.h>
#include <WiFi.h>
#include "setTime.h"  // Import tệp setTime.h

// Địa chỉ MAC của thiết bị nhận - thay đổi theo địa chỉ MAC của thiết bị đích
uint8_t broadcastAddress[] = {0x84, 0xFC, 0xE6, 0x00, 0xE7, 0xC0};  // ESP C3
// uint8_t broadcastAddress[] = {0x34, 0x98, 0x7A, 0xBA, 0xB6, 0x74};  // ESP CAM
struct TimeRequest {
  unsigned int timeOn;
  unsigned int timeOff;
};
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

void EspNowsetup() {
  Serial.begin(115200);

  // Set Wi-Fi mode to Station
  WiFi.mode(WIFI_STA);
  delay(1000); // Small delay for Wi-Fi setup
  Serial.println("WiFi mode set to WIFI_STA");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialized successfully");

  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Add peer information
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer added successfully");

  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);

  // Configure buttons for timeOn, timeOff, and setTime
  // setupButtons(4, 5, 6); // GPIO pins 4, 5, and 6 for the buttons

  // Initial values for timeOn and timeOff
  // timeOn = 0;
  // timeOff = 0;

  Serial.println("Setup completed");
}



void sendMessage(const char *message) {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.print("Error sending the data, code: ");
    Serial.println(result);  // In mã lỗi
  }
}

// void loop() {
//   // Check if the "set time" button was pressed
//   if (setTimeFlag) {
//     setTimeFlag = false; // Clear the flag

//     // Create a TimeRequest structure with the current timeOn and timeOff values
//     TimeRequest timeRequest = createTimeRequest();

//     // Log the current settings to Serial Monitor
//     Serial.println("Sending TimeRequest...");
//     Serial.print("timeOn: ");
//     Serial.println(timeRequest.timeOn);
//     Serial.print("timeOff: ");
//     Serial.println(timeRequest.timeOff);

//     // Send the TimeRequest via ESP-NOW
//     sendMessage(timeRequest);
//   }

  // Optionally, print the current `timeOn` and `timeOff` values periodically
  // static unsigned long lastPrintTime = 0;
  // if (millis() - lastPrintTime > 5000) { // Print every 5 seconds
  //   lastPrintTime = millis();
  //   Serial.print("Current timeOn: ");
  //   Serial.println(timeOn);
  //   Serial.print("Current timeOff: ");
  //   Serial.println(timeOff);
  // }
// }

