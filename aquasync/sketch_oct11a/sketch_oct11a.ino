#include <OneWire.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// =====================================================
// 1. WIFI CONFIGURATION
// =====================================================

const char* WIFI_SSID = "FTTH-29";
const char* WIFI_PASSWORD = "jr290129";

// =====================================================
// 2. HIVEMQ CLOUD MQTT CONFIGURATION
// =====================================================

const char* MQTT_SERVER =
  "aa595fb1583d4754a76c3f3ff4fbb9e3.s1.eu.hivemq.cloud";

const int MQTT_PORT = 8883;

const char* MQTT_USERNAME = "florasync";
const char* MQTT_PASSWORD = "florasync@123";

const char* TURBIDITY_TOPIC = "aquasync/turbidity";
const char* PH_TOPIC = "aquasync/ph";
const char* TEMPERATURE_TOPIC = "aquasync/temperature";

// =====================================================
// 3. SENSOR CONFIGURATION
// =====================================================

#define TURBIDITY_PIN 34
#define PH_PIN 35
#define DS18B20_PIN 4

// Relay module control (ESP32 GPIO only drives the relay input)
#define RELAY_PIN 27
#define RELAY_ON_LEVEL HIGH  // Set HIGH as the relay activation level for this module; change to LOW only if testing proves active-LOW
#define RELAY_OFF_LEVEL (RELAY_ON_LEVEL == LOW ? HIGH : LOW)

// Turbidity hysteresis thresholds: turn pump ON when very cloudy,
// and OFF after the reading improves. Tune using your own calibration.
const int PUMP_ON_TURBIDITY_LIMIT = 2150;
const int PUMP_OFF_TURBIDITY_LIMIT = 2370;
bool pumpIsOn = false;

OneWire oneWire(DS18B20_PIN);
DallasTemperature temperatureSensor(&oneWire);

const int NUM_READINGS = 10;
const int PH_SAMPLES = 20;

// Turbidity thresholds
const int VERY_CLOUDY_LIMIT = 2150;
const int CLOUDY_LIMIT = 2370;
const int MODERATELY_CLOUDY_LIMIT = 2500;
const int SLIGHTLY_CLOUDY_LIMIT = 2520;

// pH demo settings — NOT CALIBRATED
const float ADC_CORRECTION = 1.081;
const float DIVIDER_RATIO = 2.0;
const float PH7_PO_VOLTAGE = 3.416;
const float VOLTS_PER_PH = 0.18;

// =====================================================
// 4. OLED DISPLAY (SH1106, 128x64 I2C)
// =====================================================

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS_PRIMARY 0x3C
#define OLED_ADDRESS_SECONDARY 0x3D

Adafruit_SH1106G oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;
uint8_t oledAddress = OLED_ADDRESS_PRIMARY;

// Latest values used by the OLED. pH is an uncalibrated estimate.
int latestTurbidityADC = 0;
float latestPH = NAN;
float latestTemperatureC = NAN;
String latestTurbidityStatus = "Waiting...";

void updateOLED() {
  if (!oledReady) return;

  oled.clearDisplay();
  oled.setTextColor(SH110X_WHITE);
  oled.setTextSize(1);

  // Put every reading in the lower half (y = 32 to 63).
  // Text size 1 is 8 pixels tall; lines are spaced 10 pixels apart.
  oled.setCursor(0, 32);
  oled.print(F("pH: "));
  if (isfinite(latestPH)) {
    oled.print(latestPH, 2);
    oled.print(F("*"));  // * means uncalibrated estimate
  } else {
    oled.print(F("wait"));
  }

  oled.setCursor(0, 42);
  oled.print(F("Water: "));
  if (isfinite(latestTemperatureC)) {
    oled.print(latestTemperatureC, 1);
    oled.print(F(" C"));
  } else {
    oled.print(F("wait"));
  }

  oled.setCursor(0, 52);
  oled.print(F("Turb: "));
  if (latestTurbidityStatus == "VERY CLOUDY") {
    oled.print(F("VERY CLOUDY"));
  } else if (latestTurbidityStatus == "MODERATELY CLOUDY") {
    oled.print(F("MOD CLOUDY"));
  } else if (latestTurbidityStatus == "SLIGHTLY CLOUDY") {
    oled.print(F("SLIGHT CLOUDY"));
  } else if (latestTurbidityStatus == "Waiting...") {
    oled.print(F("WAIT"));
  } else {
    oled.print(latestTurbidityStatus);
  }

  oled.display();
}

// =====================================================
// 5. MQTT CLIENT
// =====================================================

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 3000;

// =====================================================
// 5. READ TURBIDITY
// =====================================================

int readTurbidity() {
  long total = 0;

  for (int i = 0; i < NUM_READINGS; i++) {
    total += analogRead(TURBIDITY_PIN);
    delay(20);
  }

  return total / NUM_READINGS;
}

const char* getTurbidityStatus(int rawADC) {
  if (rawADC < VERY_CLOUDY_LIMIT) {
    return "VERY CLOUDY";
  } else if (rawADC < CLOUDY_LIMIT) {
    return "CLOUDY";
  } else if (rawADC < MODERATELY_CLOUDY_LIMIT) {
    return "MODERATELY CLOUDY";
  } else if (rawADC < SLIGHTLY_CLOUDY_LIMIT) {
    return "SLIGHTLY CLOUDY";
  } else {
    return "CLEAR";
  }
}

// =====================================================
// 6. READ pH SENSOR
// =====================================================

float readPH() {
  long total = 0;

  for (int i = 0; i < PH_SAMPLES; i++) {
    total += analogRead(PH_PIN);
    delay(20);
  }

  float adc = total / (float)PH_SAMPLES;

  // Approximate voltage with your current correction factor
  float gpioVoltage =
      (adc * 3.3f / 4095.0f) * ADC_CORRECTION;

  // Estimate the voltage before the 10k/10k divider
  float poVoltage = gpioVoltage * DIVIDER_RATIO;

  // Provisional demo formula; calibration is unverified
  float ph = 7.0f +
             (PH7_PO_VOLTAGE - poVoltage) / VOLTS_PER_PH;

  // Keep the output within a reasonable display range
  if (ph < 0.0f) ph = 0.0f;
  if (ph > 14.0f) ph = 14.0f;

  Serial.print("pH ADC: ");
  Serial.println(adc, 0);

  Serial.print("GPIO35 voltage: ");
  Serial.print(gpioVoltage, 3);
  Serial.println(" V");

  Serial.print("Estimated PO voltage: ");
  Serial.print(poVoltage, 3);
  Serial.println(" V");

  Serial.print("Estimated pH (DEMO): ");
  Serial.println(ph, 2);

  return ph;
}

// =====================================================
// 7. CONNECT TO WIFI
// =====================================================

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// 8. CONNECT TO HIVEMQ
// =====================================================

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to HiveMQ Cloud...");

    String clientId = "AquaSync-ESP32-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (mqttClient.connect(
          clientId.c_str(),
          MQTT_USERNAME,
          MQTT_PASSWORD)) {

      Serial.println(" connected!");
      Serial.println("Turbidity topic: aquasync/turbidity");
      Serial.println("pH topic: aquasync/ph");
      Serial.println("Temperature topic: aquasync/temperature");

    } else {
      Serial.print(" failed. MQTT state: ");
      Serial.println(mqttClient.state());
      Serial.println("Retrying in 3 seconds...");
      delay(3000);
    }
  }
}

// =====================================================
// AUTOMATIC PUMP CONTROL
// =====================================================
void controlPumpFromTurbidity(int rawADC) {
  // Lower ADC readings indicate cloudier water in this prototype.
  // Hysteresis avoids rapid relay chatter around one threshold.
  if (!pumpIsOn && rawADC < PUMP_ON_TURBIDITY_LIMIT) {
    pumpIsOn = true;
    digitalWrite(RELAY_PIN, RELAY_ON_LEVEL);
    Serial.println("Turbidity high: pump ON");
  } else if (pumpIsOn && rawADC >= PUMP_OFF_TURBIDITY_LIMIT) {
    pumpIsOn = false;
    digitalWrite(RELAY_PIN, RELAY_OFF_LEVEL);
    Serial.println("Turbidity improved: pump OFF");
  } else {
    Serial.print("Pump state unchanged: ");
    Serial.println(pumpIsOn ? "ON" : "OFF");
  }
}

// =====================================================
// 9. PUBLISH TURBIDITY DATA
// =====================================================

void publishTurbidityData() {
  int rawADC = readTurbidity();

  float adcVoltage = (rawADC / 4095.0f) * 3.3f;
  float sensorVoltage = adcVoltage * 2.0f;

  const char* status = getTurbidityStatus(rawADC);
  latestTurbidityADC = rawADC;
  latestTurbidityStatus = status;
  controlPumpFromTurbidity(rawADC);
  updateOLED();

  char payload[200];

  snprintf(
    payload,
    sizeof(payload),
    "{\"rawADC\":%d,\"adcVoltage\":%.3f,"
    "\"sensorVoltage\":%.3f,\"status\":\"%s\"}",
    rawADC,
    adcVoltage,
    sensorVoltage,
    status
  );

  Serial.println("\n========== TURBIDITY ==========");
  Serial.print("Raw ADC: ");
  Serial.println(rawADC);
  Serial.print("Turbidity status: ");
  Serial.println(status);
  Serial.print("MQTT payload: ");
  Serial.println(payload);

  if (mqttClient.publish(TURBIDITY_TOPIC, payload, false)) {
    Serial.println("Turbidity publish successful!");
  } else {
    Serial.println("Turbidity publish failed!");
  }
}

// =====================================================
// 10. PUBLISH pH DATA
// =====================================================

void publishPHData() {
  float ph = readPH();
  latestPH = ph;
  updateOLED();

  char payload[100];

  snprintf(
    payload,
    sizeof(payload),
    "{\"ph\":%.2f,\"calibrated\":false}",
    ph
  );

  Serial.print("pH MQTT payload: ");
  Serial.println(payload);

  if (mqttClient.publish(PH_TOPIC, payload, false)) {
    Serial.println("pH publish successful!");
  } else {
    Serial.println("pH publish failed!");
  }
}

// =====================================================
// 11. PUBLISH DS18B20 WATER TEMPERATURE
// =====================================================

void publishTemperatureData() {
  temperatureSensor.requestTemperatures();
  float temperatureC = temperatureSensor.getTempCByIndex(0);

  if (temperatureC == DEVICE_DISCONNECTED_C || !isfinite(temperatureC)) {
    Serial.println("DS18B20 error: sensor not detected; skipping publish.");
    return;
  }

  latestTemperatureC = temperatureC;
  updateOLED();

  char payload[64];

  snprintf(payload, sizeof(payload), "{\"temperatureC\":%.2f}", temperatureC);
  Serial.print("Water temperature: ");
  Serial.print(temperatureC, 2);
  Serial.println(" °C");
  Serial.print("Temperature MQTT payload: ");
  Serial.println(payload);

  if (mqttClient.publish(TEMPERATURE_TOPIC, payload, false)) {
    Serial.println("Temperature publish successful!");
  } else {
    Serial.println("Temperature publish failed!");
  }
}

// =====================================================
// 12. SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C setup for a classic ESP32 DevKit: SDA=21, SCL=22.
  Wire.begin(21, 22);
  Wire.setClock(100000);

  // Scan the I2C bus and report detected addresses in Serial Monitor.
  bool found3C = false;
  bool found3D = false;
  Serial.println(F("Scanning I2C bus for OLED..."));
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("I2C device found at 0x"));
      if (address < 16) Serial.print('0');
      Serial.println(address, HEX);
      if (address == OLED_ADDRESS_PRIMARY) found3C = true;
      if (address == OLED_ADDRESS_SECONDARY) found3D = true;
    }
  }

  if (found3C) oledAddress = OLED_ADDRESS_PRIMARY;
  else if (found3D) oledAddress = OLED_ADDRESS_SECONDARY;

  if (found3C || found3D) {
    oledReady = oled.begin(oledAddress, true);
  }

  if (oledReady) {
    Serial.print(F("OLED initialized at I2C address 0x"));
    Serial.println(oledAddress, HEX);
    oled.clearDisplay();
    oled.setTextColor(SH110X_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 32);
    oled.println(F("AquaSync starting"));
    oled.setCursor(0, 44);
    oled.println(F("Reading sensors..."));
    oled.display();
  } else {
    Serial.println(F("OLED not detected/initialized. Check power, GND, SDA=21, SCL=22, and display type."));
    Serial.println(F("If scanner finds no I2C device, fix wiring before expecting data on screen."));
  }

  // Start pump OFF before enabling automatic control.
  digitalWrite(RELAY_PIN, RELAY_OFF_LEVEL);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF_LEVEL);

  pinMode(TURBIDITY_PIN, INPUT);
  pinMode(PH_PIN, INPUT);
  temperatureSensor.begin();
  Serial.print("DS18B20 sensors detected: ");
  Serial.println(temperatureSensor.getDeviceCount());

  analogReadResolution(12);

  // Prototype setting: encrypted connection without
  // server-certificate verification.
  secureClient.setInsecure();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);
  mqttClient.setSocketTimeout(10);

  Serial.println("\n================================");
  Serial.println("       AQUASYNC STARTING");
  Serial.println("================================");

  Serial.println("pH calculation: DEMO ONLY");
  Serial.println("pH calibration: NOT VERIFIED");
  Serial.println("Automatic pump relay: GPIO 27");
  Serial.println("Pump ON below turbidity ADC 2150; OFF at/above 2370.");

  connectWiFi();
  connectMQTT();

  Serial.println("AquaSync is ready!");
  updateOLED();
}

// =====================================================
// 12. MAIN LOOP
// =====================================================

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected!");
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  unsigned long currentTime = millis();

  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = currentTime;

    publishTurbidityData();
    publishPHData();
    publishTemperatureData();
    updateOLED();
  }
}
