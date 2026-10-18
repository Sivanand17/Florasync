#include <WiFi.h>

#include <WiFiClientSecure.h>

#include <PubSubClient.h>

unsigned long lastHeartbeat = 0;

// =====================================================

// WIFI

// =====================================================

const char* ssid = "FTTH-29";

const char* wifiPassword = "jr290129";

// =====================================================

// HIVEMQ CLOUD

// =====================================================

const char* mqttServer =

  "aa595fb1583d4754a76c3f3ff4fbb9e3.s1.eu.hivemq.cloud";

const int mqttPort = 8883;

const char* mqttUsername = "florasync";

const char* mqttPassword = "florasync@123";

// =====================================================

// MQTT TOPIC

// =====================================================

const char* movementTopic = "agribot/movement";
const char* motor1Topic = "agribot/motor1";
const char* motor2Topic = "agribot/motor2";

// =====================================================

// MOTOR PINS

// =====================================================

#define LA 26

#define LB 27

#define RA 14

#define RB 12

// WATER PUMP RELAY PINS
#define PUMP1_RELAY 32
#define PUMP2_RELAY 33

// Active-LOW relay modules
#define RELAY_ON LOW
#define RELAY_OFF HIGH

// =====================================================

// MQTT

// =====================================================

WiFiClientSecure espClient;

PubSubClient mqttClient(espClient);

// =====================================================

// SAFETY TIMEOUT

// =====================================================

// If communication stops, automatically stop the motors.

unsigned long lastMovementCommand = 0;

const unsigned long MOTOR_TIMEOUT = 1500;

// =====================================================

// MOTOR STOP

// =====================================================

void motorStop() {

  digitalWrite(LA, LOW);

  digitalWrite(LB, LOW);

  digitalWrite(RA, LOW);

  digitalWrite(RB, LOW);

}

// =====================================================

// FORWARD

// =====================================================

void forward() {

  digitalWrite(LA, HIGH);

  digitalWrite(LB, LOW);

  digitalWrite(RA, LOW);

  digitalWrite(RB, HIGH);

}

// =====================================================

// BACKWARD

// =====================================================

void backward() {

  digitalWrite(LA, LOW);

  digitalWrite(LB, HIGH);

  digitalWrite(RA, HIGH);

  digitalWrite(RB, LOW);

}

// =====================================================

// LEFT

// =====================================================

void left() {

  // Left side STOP

  digitalWrite(LA, LOW);

  digitalWrite(LB, LOW);

  // Right side FORWARD

  digitalWrite(RA, LOW);

  digitalWrite(RB, HIGH);

}

// =====================================================

// RIGHT

// =====================================================

void right() {

  // Left side FORWARD

  digitalWrite(LA, HIGH);

  digitalWrite(LB, LOW);

  // Right side STOP

  digitalWrite(RA, LOW);

  digitalWrite(RB, LOW);

}

// =====================================================

// MQTT CALLBACK

// =====================================================

void mqttCallback(

  char* topic,

  byte* payload,

  unsigned int length

) {

  String command = "";

  // Convert MQTT payload to String

  for (unsigned int i = 0; i < length; i++) {

    command += (char)payload[i];

  }

  Serial.println();

  Serial.println("================================");

  Serial.println("MQTT MESSAGE RECEIVED");

  Serial.println("================================");

  Serial.print("Topic: ");

  Serial.println(topic);

  Serial.print("Command: [");

  Serial.print(command);

  Serial.println("]");

  // ===================================================
  // WATER PUMP CONTROL (dashboard topics: ON/OFF)
  // ===================================================
  if (String(topic) == motor1Topic) {
    if (command == "ON") {
      digitalWrite(PUMP1_RELAY, RELAY_ON);
      Serial.println("Water Pump 1 -> ON");
    } else if (command == "OFF") {
      digitalWrite(PUMP1_RELAY, RELAY_OFF);
      Serial.println("Water Pump 1 -> OFF");
    } else {
      Serial.println("Unknown Pump 1 command; state unchanged");
    }
  }
  else if (String(topic) == motor2Topic) {
    if (command == "ON") {
      digitalWrite(PUMP2_RELAY, RELAY_ON);
      Serial.println("Water Pump 2 -> ON");
    } else if (command == "OFF") {
      digitalWrite(PUMP2_RELAY, RELAY_OFF);
      Serial.println("Water Pump 2 -> OFF");
    } else {
      Serial.println("Unknown Pump 2 command; state unchanged");
    }
  }
  // ===================================================
  // WHEEL MOTOR MOVEMENT
  // ===================================================
  else if (String(topic) == movementTopic && command == "FORWARD") {

    forward();

    lastMovementCommand = millis();

    Serial.println("Motor -> FORWARD");

  }

  // ===================================================

  // BACKWARD

  // ===================================================

  else if (String(topic) == movementTopic && command == "BACKWARD") {

    backward();

    lastMovementCommand = millis();

    Serial.println("Motor -> BACKWARD");

  }

  // ===================================================

  // LEFT

  // ===================================================

  else if (String(topic) == movementTopic && command == "LEFT") {

    left();

    lastMovementCommand = millis();

    Serial.println("Motor -> LEFT");

  }

  // ===================================================

  // RIGHT

  // ===================================================

  else if (String(topic) == movementTopic && command == "RIGHT") {

    right();

    lastMovementCommand = millis();

    Serial.println("Motor -> RIGHT");

  }

  // ===================================================

  // STOP

  // ===================================================

  else if (String(topic) == movementTopic && command == "STOP") {

    motorStop();

    lastMovementCommand = millis();

    Serial.println("Motor -> STOP");

  }

  // ===================================================

  // UNKNOWN

  // ===================================================

  else if (String(topic) == movementTopic) {

    Serial.println("Unknown movement command!");

    motorStop();

  }

}

// =====================================================

// CONNECT TO WIFI

// =====================================================

void connectWiFi() {

  Serial.println();

  Serial.println("================================");

  Serial.println("CONNECTING TO WIFI");

  Serial.println("================================");

  WiFi.mode(WIFI_STA);

  WiFi.begin(

    ssid,

    wifiPassword

  );

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  Serial.println();

  Serial.println("WIFI CONNECTED!");

  Serial.print("ESP32 IP: ");

  Serial.println(WiFi.localIP());

  Serial.print("WiFi Signal: ");

  Serial.print(WiFi.RSSI());

  Serial.println(" dBm");

}

// =====================================================

// CONNECT TO HIVEMQ

// =====================================================

void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.println();

    Serial.println("================================");

    Serial.println("CONNECTING TO HIVEMQ");

    Serial.println("================================");

    String clientID =

      "AgriBot-ESP32-" +

      String((uint32_t)ESP.getEfuseMac(), HEX);

    Serial.print("Client ID: ");

    Serial.println(clientID);

    Serial.print("Connecting... ");

    if (

      mqttClient.connect(

        clientID.c_str(),

        mqttUsername,

        mqttPassword

      )

    ) {

      Serial.println("CONNECTED!");

      // Subscribe to movement topic

      bool subscribed = mqttClient.subscribe(

        movementTopic,

        1

      );

      if (subscribed) {

        Serial.println("MQTT SUBSCRIBE SUCCESS!");

        Serial.print("Topic: ");

        Serial.println(movementTopic);

      } else {

        Serial.println("MQTT SUBSCRIBE FAILED!");

      }

      bool pump1Subscribed = mqttClient.subscribe(motor1Topic, 1);
      bool pump2Subscribed = mqttClient.subscribe(motor2Topic, 1);
      Serial.println(pump1Subscribed ? "Pump 1 topic subscribed" : "Pump 1 topic subscribe FAILED");
      Serial.println(pump2Subscribed ? "Pump 2 topic subscribed" : "Pump 2 topic subscribe FAILED");

      // Always start stopped

      motorStop();

      lastMovementCommand = millis();

    }

    else {

      Serial.println("FAILED!");

      Serial.print("MQTT State: ");

      Serial.println(mqttClient.state());

      Serial.println("Retrying in 2 seconds...");

      motorStop();

      delay(2000);

    }

  }

}

// =====================================================

// SETUP

// =====================================================

void setup() {

  Serial.begin(115200);

  delay(2000);

  Serial.println();

  Serial.println();

  Serial.println("################################");

  Serial.println("#                              #");

  Serial.println("#       AGRIBOT ESP32          #");

  Serial.println("#       MOTOR CONTROLLER       #");

  Serial.println("#                              #");

  Serial.println("################################");

  // ===================================================

  // MOTOR PINS

  // ===================================================

  Serial.println();

  Serial.println("Initializing motor pins...");

  pinMode(LA, OUTPUT);

  pinMode(LB, OUTPUT);

  pinMode(RA, OUTPUT);

  pinMode(RB, OUTPUT);

  pinMode(PUMP1_RELAY, OUTPUT);
  pinMode(PUMP2_RELAY, OUTPUT);
  digitalWrite(PUMP1_RELAY, RELAY_OFF);
  digitalWrite(PUMP2_RELAY, RELAY_OFF);

  motorStop();

  Serial.println("Motor pins initialized.");

  Serial.println("Motors stopped.");

  // ===================================================

  // WIFI

  // ===================================================

  connectWiFi();

  // ===================================================

  // TLS

  // ===================================================

  Serial.println();

  Serial.println("Configuring TLS...");

  // Temporary testing configuration.

  // We can add certificate validation later.

  espClient.setInsecure();

  Serial.println("TLS configured.");

  // ===================================================

  // MQTT

  // ===================================================

  mqttClient.setServer(

    mqttServer,

    mqttPort

  );

  mqttClient.setCallback(

    mqttCallback

  );

  // MQTT connection settings

  mqttClient.setKeepAlive(30);

  mqttClient.setSocketTimeout(10);

  mqttClient.setBufferSize(512);

  // ===================================================

  // CONNECT

  // ===================================================

  connectMQTT();

  // ===================================================

  // READY

  // ===================================================

  Serial.println();

  Serial.println("################################");

  Serial.println("#                              #");

  Serial.println("#         AGRIBOT READY        #");

  Serial.println("#                              #");

  Serial.println("################################");

}

// =====================================================

// LOOP

// =====================================================

void loop() {

  // ===================================================

  // WIFI CHECK

  // ===================================================

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected!");

    motorStop();

    connectWiFi();

  }

  // ===================================================

  // MQTT CHECK

  // ===================================================

  if (!mqttClient.connected()) {

    Serial.println("MQTT disconnected!");

    motorStop();

    connectMQTT();

  }

  // ===================================================

  // PROCESS MQTT

  // ===================================================

  mqttClient.loop();

  // ===================================================

  // MOTOR SAFETY

  // ===================================================

  if (

    millis() - lastMovementCommand >

    MOTOR_TIMEOUT

  ) {

    motorStop();

  }

  // ===================================================

  // ROBOT HEARTBEAT

  // ===================================================

  if (

    millis() - lastHeartbeat >= 2000

  ) {

    lastHeartbeat = millis();

    if (mqttClient.connected()) {

      bool success = mqttClient.publish(

        "agribot/status/online",

        "ONLINE",

        false

      );

      if (success) {

        Serial.println(

          "💚 HEARTBEAT SENT: ONLINE"

        );

      } else {

        Serial.println(

          "❌ HEARTBEAT FAILED"

        );

      }

    } else {

      Serial.println(

        "❌ Cannot send heartbeat - MQTT disconnected"

      );

    }

  }

  delay(5);

}