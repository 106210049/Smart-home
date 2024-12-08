#include "RMaker.h"
#include "WiFiProv.h"
#include <SimpleTimer.h>

const char *service_name = "PROV_SmartHome";
const char *pop = "1234";

// define the Chip Id
uint32_t espChipId = 5;

// define the Node Name
char nodeName[] = "ESP32_Smarthome";
static uint8_t wifiLed      = 2;   //D2
static uint8_t gpio_reset   = 0;

SimpleTimer Timer;
static TemperatureSensor temperature("Temperature");
static TemperatureSensor humidity("Humidity");
static TemperatureSensor ldr("LDR");
static TemperatureSensor gas("Gas level");

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {      
        case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
#endif        
        break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.printf("\nConnected to Wi-Fi!\n");
        digitalWrite(wifiLed, true);
        break;
    }
}

void sendSensor(float Temperature,float Humidity, float LightVal, float gasLevel)
{
  temperature.updateAndReportParam("Temperature", Temperature);
  humidity.updateAndReportParam("Temperature", Humidity);
  ldr.updateAndReportParam("Temperature", LightVal);
  gas.updateAndReportParam("Temperature", gasLevel);
}


void RainmakerSetup()
{
  pinMode(wifiLed, OUTPUT);

  pinMode(gpio_reset, INPUT);

  digitalWrite(wifiLed, LOW);

  Node my_node;
  my_node = RMaker.initNode(nodeName);
  
  //Add switch device to the node
  my_node.addDevice(temperature);
  my_node.addDevice(humidity);
  my_node.addDevice(ldr);
  my_node.addDevice(gas);

  Timer.setInterval(2000);

  //This is optional
  
  RMaker.enableTZService();
  RMaker.enableSchedule();

  //Service Name
  for (int i = 0; i < 17; i = i + 8) {
    espChipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  Serial.printf("nChip ID:  %d Service Name: %sn", espChipId, service_name);

  Serial.println("nStarting ESP-RainMakern");
  RMaker.start();
  
  WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

}

void RainMakerActive(String temperature,String humidity, float lightValue, float gasVal){
  // Read GPIO0 (external button to reset device
  float Temperature=temperature.toFloat();
  float Humidity= humidity.toFloat();
  if (digitalRead(gpio_reset) == LOW) { //Push button pressed
    Serial.printf("Reset Button Pressed!n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.n");
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.n");
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    }
  }
  delay(100);

  if (WiFi.status() != WL_CONNECTED)
  {
    //Serial.println("WiFi Not Connected");
    digitalWrite(wifiLed, false);
  }
  else
  {
    //Serial.println("WiFi Connected");
    digitalWrite(wifiLed, true);
    if (Timer.isReady()) {
      //Serial.println("Sending Sensor Data");
      sendSensor(Temperature,Humidity,lightValue,gasVal);
      Timer.reset();      // Reset a second timer
    }
  }
}
