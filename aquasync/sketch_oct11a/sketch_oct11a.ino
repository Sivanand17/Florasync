
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

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

// AquaSync topic — must match your React dashboard
const char* MQTT_TOPIC = "aquasync/turbidity";

// =====================================================
// 3. TURBIDITY SENSOR CONFIGURATION
// =====================================================

#define TURBIDITY_PIN 34

const int NUM_READINGS = 10;

// Experimental ADC thresholds
const int VERY_CLOUDY_LIMIT = 2150;
const int CLOUDY_LIMIT = 2370;
const int MODERATELY_CLOUDY_LIMIT = 2500;
const int SLIGHTLY_CLOUDY_LIMIT = 2520;

// =====================================================
// 4. MQTT CLIENT
// =====================================================

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// Publish interval
unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 3000;

// =====================================================
// 5. READ TURBIDITY SENSOR
// =====================================================

int readTurbidity() {
  long total = 0;

  for (int i = 0; i < NUM_READINGS; i++) {
    total += analogRead(TURBIDITY_PIN);
    delay(20);
  }

  return total / NUM_READINGS;
}

// =====================================================
// 6. CLASSIFY WATER TURBIDITY
// =====================================================

const char* getTurbidityStatus(int rawADC) {
  if (rawADC < VERY_CLOUDY_LIMIT) {
    return "VERY CLOUDY";
  }
  else if (rawADC < CLOUDY_LIMIT) {
    return "CLOUDY";
  }
  else if (rawADC < MODERATELY_CLOUDY_LIMIT) {
    return "MODERATELY CLOUDY";
  }
  else if (rawADC < SLIGHTLY_CLOUDY_LIMIT) {
    return "SLIGHTLY CLOUDY";
  }
  else {
    return "CLEAR";
  }
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

  Serial.println();
  Serial.println("Wi-Fi connected!");

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
      Serial.print("Publishing to topic: ");
      Serial.println(MQTT_TOPIC);

    } else {
      Serial.print(" failed. MQTT state: ");
      Serial.println(mqttClient.state());

      Serial.println("Retrying in 3 seconds...");
      delay(3000);
    }
  }
}

// =====================================================
// 9. PUBLISH AQUASYNC DATA
// =====================================================

void publishTurbidityData() {
  int rawADC = readTurbidity();

  // Approximate ESP32 ADC pin voltage
  float adcVoltage = (rawADC / 4095.0f) * 3.3f;

  // Voltage before the 1:1 voltage divider.
  // Assumes two equal 10k resistors.
  float sensorVoltage = adcVoltage * 2.0f;

  const char* status = getTurbidityStatus(rawADC);

  // Construct JSON payload
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

  Serial.println();
  Serial.println("========== AQUASYNC ==========");

  Serial.print("Raw ADC: ");
  Serial.println(rawADC);

  Serial.print("ADC voltage: ");
  Serial.print(adcVoltage, 3);
  Serial.println(" V");

  Serial.print("Estimated sensor voltage: ");
  Serial.print(sensorVoltage, 3);
  Serial.println(" V");

  Serial.print("Turbidity status: ");
  Serial.println(status);

  Serial.print("MQTT payload: ");
  Serial.println(payload);

  if (mqttClient.publish(MQTT_TOPIC, payload, false)) {
    Serial.println("MQTT publish successful!");
  } else {
    Serial.println("MQTT publish failed!");
  }

  Serial.println("==============================");
}

// =====================================================
// 10. SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(TURBIDITY_PIN, INPUT);

  // Use 12-bit ADC readings: 0 to 4095
  analogReadResolution(12);

  // Prototype TLS configuration:
  // Encrypts the connection but does not verify
  // the server certificate. See security note below.
  secureClient.setInsecure();

  // Configure HiveMQ connection
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);
  mqttClient.setSocketTimeout(10);

  Serial.println();
  Serial.println("================================");
  Serial.println("       AQUASYNC STARTING        ");
  Serial.println("================================");

  connectWiFi();
  connectMQTT();

  Serial.println("AquaSync is ready!");
}

// =====================================================
// 11. MAIN LOOP
// =====================================================

void loop() {
  // Reconnect to Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected!");
    connectWiFi();
  }

  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  // Publish every 3 seconds
  unsigned long currentTime = millis();

  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = currentTime;
    publishTurbidityData();
  }
}
