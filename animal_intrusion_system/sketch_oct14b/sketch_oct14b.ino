
#include <SoftwareSerial.h>

SoftwareSerial GSM(4, 5);  // Arduino RX, TX

const char PHONE[] = "+916282047605";
const unsigned long ALERT_COOLDOWN = 60000;

unsigned long lastAlertTime = 0;
bool alertSent = false;

String readGSMResponse(unsigned long timeout) {
  String response = "";
  unsigned long start = millis();

  while (millis() - start < timeout) {
    while (GSM.available()) {
      char c = GSM.read();
      response += c;

      if (response.indexOf("ERROR") >= 0 ||
          response.indexOf("OK") >= 0) {
        return response;
      }
    }
  }
  return response;
}

bool sendSMS(String animal) {
  GSM.println("AT+CMGF=1");

  String response = readGSMResponse(3000);
  if (response.indexOf("OK") < 0) {
    return false;
  }

  GSM.print("AT+CMGS=\"");
  GSM.print(PHONE);
  GSM.println("\"");

  unsigned long start = millis();
  bool promptReceived = false;

  while (millis() - start < 5000) {
    while (GSM.available()) {
      if (GSM.read() == '>') {
        promptReceived = true;
        break;
      }
    }
    if (promptReceived) break;
  }

  if (!promptReceived) return false;

  GSM.print("FloraSync Alert! ");
  GSM.print(animal);
  GSM.println(" detected. Please check the farm.");

  GSM.write(26);  // Ctrl+Z to send SMS

  response = readGSMResponse(15000);
  return response.indexOf("+CMGS:") >= 0;
}

void setup() {
  Serial.begin(9600);
  GSM.begin(9600);

  delay(1000);
  GSM.println("AT");
  readGSMResponse(2000);

  GSM.println("ATE0");
  readGSMResponse(2000);
}

void loop() {
  if (Serial.available()) {
    String animal = Serial.readStringUntil('\n');
    animal.trim();
    animal.toUpperCase();

    if (animal != "BOAR" &&
        animal != "ELEPHANT" &&
        animal != "TIGER") {
      return;
    }

    if (alertSent &&
        millis() - lastAlertTime < ALERT_COOLDOWN) {
      return;
    }

    if (sendSMS(animal)) {
      lastAlertTime = millis();
      alertSent = true;
    }
  }
}
