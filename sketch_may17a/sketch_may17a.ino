#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= PINS =================

#define SOIL_PIN 34
#define RELAY_PIN 26

// ================= WIFI =================

const char* ssid = "FTTH-29";
const char* password = "jr290129";

// ================= MQTT =================

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// ================= LCD =================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= CONTROL MODE =================

String controlMode = "AUTO";

// ================= MQTT CALLBACK =================

void callback(char* topic, byte* message, unsigned int length) {

  String msg = "";

  for (unsigned int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  msg.trim();

  Serial.print("MQTT Message: ");
  Serial.println(msg);

  if (msg == "ON") {
    controlMode = "ON";
  }
  else if (msg == "OFF") {
    controlMode = "OFF";
  }
  else if (msg == "AUTO") {
    controlMode = "AUTO";
  }
}

// ================= WIFI CONNECT =================

void setup_wifi() {

  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT RECONNECT =================

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    String clientId = "ESP32Farm-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println("Connected");

      client.subscribe("smartfarm/control");

    } else {

      Serial.print("Failed, rc=");
      Serial.println(client.state());

      delay(5000);
    }
  }
}

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  pinMode(SOIL_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);

  // Relay OFF initially
  digitalWrite(RELAY_PIN, HIGH);

  // I2C
  Wire.begin(21, 22);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Farming");

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(2000);
  lcd.clear();

  // WiFi
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// ================= LOOP =================

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // ================= SOIL SENSOR =================

  long total = 0;

  for (int i = 0; i < 10; i++) {
    total += analogRead(SOIL_PIN);
    delay(5);
  }

  int soilValue = total / 10;

  // Calibration:
  // Dry = 4095
  // Wet = 2000

  int moisturePercent = map(
    soilValue,
    4095,
    2000,
    0,
    100
  );

  moisturePercent = constrain(
    moisturePercent,
    0,
    100
  );

  // ================= PUMP CONTROL =================

  static bool pumpState = false;

  String pumpStatus;

  if (controlMode == "ON") {

    pumpState = true;
  }
  else if (controlMode == "OFF") {

    pumpState = false;
  }
  else {

    // AUTO mode with hysteresis

    if (moisturePercent <= 30) {
      pumpState = true;
    }

    if (moisturePercent >= 40) {
      pumpState = false;
    }
  }

  digitalWrite(
    RELAY_PIN,
    pumpState ? LOW : HIGH
  );

  pumpStatus = pumpState ? "ON" : "OFF";

  // ================= LCD =================

  lcd.setCursor(0, 0);
  lcd.print("Soil:          ");

  lcd.setCursor(6, 0);
  lcd.print("     ");

  lcd.setCursor(6, 0);
  lcd.print(moisturePercent);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Pump:          ");

  lcd.setCursor(6, 1);
  lcd.print("     ");

  lcd.setCursor(6, 1);
  lcd.print(pumpStatus);

  // ================= SERIAL =================

  Serial.println("==============");

  Serial.print("Raw Soil Value: ");
  Serial.println(soilValue);

  Serial.print("Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  Serial.print("Pump Status: ");
  Serial.println(pumpStatus);

  Serial.print("Mode: ");
  Serial.println(controlMode);

  // ================= MQTT JSON =================

  String payload = "{";
  payload += "\"soil\":";
  payload += String(moisturePercent);
  payload += ",";
  payload += "\"pump\":\"";
  payload += pumpStatus;
  payload += "\",";
  payload += "\"mode\":\"";
  payload += controlMode;
  payload += "\"";
  payload += "}";

  client.publish(
    "sivanand/farm/data",
    payload.c_str()
  );

  Serial.print("MQTT Sent: ");
  Serial.println(payload);

  delay(5000);
}
