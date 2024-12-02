#include <Arduino.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>
#include "DHT11.h"       // DHT11 sensor lib
#include "lightSensor.h" // Light sensor lib
#include "MQ2.h"         // Gas sensor lib
#include "RealTime.h"    // RealTime lib
#include "LCD1602.h"     // LCD16x02 lib
#include "emergency.h"   // Emergency mode lib
#include "motor.h"       // Motor driver lib

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
Motor motorController;

RTC_DATA_ATTR bool modeState = NORMAL;
RTC_DATA_ATTR bool modetmp = NORMAL;


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Sensor Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      text-align: center;
      background-color: #f7f7f7;
    }
    h1 {
      background-color: #4CAF50;
      color: white;
      padding: 20px;
      margin: 0;
    }
    .container {
      display: grid;
      grid-template-columns: repeat(2, 1fr); /* 2 cột */
      grid-gap: 20px; /* Khoảng cách giữa các phần tử */
      margin: 20px auto;
      padding: 0 20px;
      max-width: 600px; /* Giới hạn chiều rộng tối đa */
    }
    .card {
      background-color: white;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
      border-radius: 8px;
      padding: 20px;
      text-align: center;
    }
    .card .title {
      font-size: 1.2rem;
      font-weight: bold;
      margin-bottom: 10px;
    }
    .card .value {
      font-size: 2rem;
      font-weight: bold;
    }
    .temperature {
      border-top: 5px solid #FF5722;
    }
    .humidity {
      border-top: 5px solid #2196F3;
    }
    .light {
      border-top: 5px solid #FFC107;
    }
    .gas {
      border-top: 5px solid #9C27B0;
    }
  </style>
</head>
<body>
  <h1>ESP32 Sensor Dashboard</h1>
  <div class="container">
    <div class="card temperature">
      <div class="title">Temperature</div>
      <div class="value"><span id="temperature">0</span> &deg;C</div>
    </div>
    <div class="card humidity">
      <div class="title">Humidity</div>
      <div class="value"><span id="humidity">0</span> %</div>
    </div>
    <div class="card light">
      <div class="title">Light Intensity</div>
      <div class="value"><span id="light">0</span></div>
    </div>
    <div class="card gas">
      <div class="title">Gas Level</div>
      <div class="value"><span id="gas">0</span></div>
    </div>
  </div>

  <script>
    if (!!window.EventSource) {
      var source = new EventSource('/events');
      source.addEventListener('sensor_data', function(e) {
        var data = JSON.parse(e.data);
        document.getElementById("temperature").innerHTML = data.temperature;
        document.getElementById("humidity").innerHTML = data.humidity;
        document.getElementById("light").innerHTML = data.light;
        document.getElementById("gas").innerHTML = data.gas;
      }, false);
    }
  </script>
</body>
</html>
)rawliteral";




// JSON object to hold sensor data
JSONVar sensorData;

// Create server
AsyncWebServer server(80);
AsyncEventSource events("/events");

void setup() {
  Serial.begin(115200);
  sensor.setupSensor();
  timer.setupTime();
  WiFi.mode(WIFI_STA);
  wifiController.connectWifi();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.setupLCD();
  em_mode.setupBuzzer();
  motorController.setupMotor();
  esp_sleep_enable_timer_wakeup(sleepDuration);

  // Set up web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  events.onConnect([](AsyncEventSourceClient* client) {
    Serial.println("Client connected!");
  });
  server.addHandler(&events);
  server.begin();
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
  Serial.printf("MQ2 Value: %d\n", sensor.gasSensor.getMQ2Value());
  Serial.printf("Temperature: %s°C\n", sensor.dht11.getTemperature());
  Serial.printf("Humidity: %s%%\n", sensor.dht11.getHumidity());
  Serial.printf("Light Intensity: %d\n", sensor.lightsensor.GetlightIntensity());
  timer.GetTime();
  timer.PrintTime();

  if (modetmp != modeState) {
    lcdController.clear();
    modetmp = modeState;  // Update modetmp to reflect the new mode
    if (modeState == NORMAL) {
      startTime = millis(); // Reset start time when entering NORMAL mode
    }
  }
  if (sensor.gasSensor.getMQ2Value() > 400) {
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
    lcd.displayLCD(sensor.dht11, timer);

    // Gửi dữ liệu lên web server
    sensorData["temperature"] = sensor.dht11.getTemperature();
    sensorData["humidity"] = sensor.dht11.getHumidity();
    sensorData["light"] = sensor.lightsensor.GetlightIntensity();
    sensorData["gas"] = sensor.gasSensor.getMQ2Value();

    static unsigned long lastTime = 0;
    unsigned long now = millis();
    if (now - lastTime > 5000) {
      String jsonString = JSON.stringify(sensorData);
      events.send(jsonString.c_str(), "sensor_data", millis());
      lastTime = now;
    }

    if ((timer.GetHour() < 5 || timer.GetHour() > 22) && millis() - startTime >= 100000) {
      enterDeepSleepMode();
    }
  }
}
