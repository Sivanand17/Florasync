#include <SoftwareSerial.h>

SoftwareSerial sim800(2, 3);   // Arduino RX=2, TX=3

const int buzzer = 8;

// Change this to your phone number
const String phoneNumber = "+919496116129";

unsigned long lastCallTime = 0;
const unsigned long callCooldown = 1000;   // 2 minutes

void setup() {

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  Serial.begin(9600);
  sim800.begin(9600);

  delay(8000);

  Serial.println("==================================");
  Serial.println(" FloraSync Animal Alert System");
  Serial.println(" Ready...");
  Serial.println("==================================");

  sim800.println("AT");
  delay(1000);

  while (sim800.available())
    Serial.write(sim800.read());
}

void loop() {

  if (Serial.available()) {

    String animal = Serial.readStringUntil('\n');

    animal.trim();
    animal.toUpperCase();

    Serial.print("Detected : ");
    Serial.println(animal);

    if (millis() - lastCallTime < callCooldown) {
      Serial.println("Call skipped (Cooldown Active)");
      return;
    }

    if (animal == "ELEPHANT") {

      callFarmer("ELEPHANT");

    }
    else if (animal == "BOAR") {

      callFarmer("BOAR");

    }
    else if (animal == "TIGER") {

      callFarmer("TIGER");

    }
    else {

      Serial.println("Unknown Animal");

    }

  }

}

void callFarmer(String animal) {

  lastCallTime = millis();

  Serial.println("\n==============================");
  Serial.println(" ALERT ");
  Serial.println("==============================");

  Serial.print("Animal : ");
  Serial.println(animal);

  // Buzzer
  digitalWrite(buzzer, HIGH);
  delay(5000);
  digitalWrite(buzzer, LOW);

  while (sim800.available())
    sim800.read();

  Serial.println("Calling Farmer...");

  sim800.println("AT");
  delay(1000);

  while (sim800.available())
    Serial.write(sim800.read());

  sim800.print("ATD");
  sim800.print(phoneNumber);
  sim800.println(";");

  Serial.println("Call Started");

  // Ring for 30 seconds
  delay(30000);

  sim800.println("ATH");

  delay(1000);

  while (sim800.available())
    Serial.write(sim800.read());

  Serial.println("Call Ended");
  Serial.println();

}