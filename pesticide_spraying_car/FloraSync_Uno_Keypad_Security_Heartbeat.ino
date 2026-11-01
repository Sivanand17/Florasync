#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <string.h>

// =====================================================
// FLORASYNC AGRIBOT - KEYPAD SECURITY + I2C LCD
// Arduino UNO
//
// LCD: I2C address 0x3F
// LCD SDA -> A4, SCL -> A5, VCC -> 5V, GND -> GND
//
// Keypad rows -> D2,D3,D4,D5
// Keypad columns -> D6,D7,D8,D9
//
// UART link:
// UNO D10 RX <- ESP32 GPIO17 TX
// UNO D11 TX -> ESP32 GPIO25 RX THROUGH A 5V-to-3.3V LEVEL SHIFTER
// UNO GND <-> ESP32 GND (COMMON GROUND REQUIRED)
// =====================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial securityLink(10, 11); // RX, TX

const char CORRECT_PIN[] = "2580"; // Change this PIN before using
const byte PIN_LENGTH = 4;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char enteredPIN[PIN_LENGTH + 1];
byte enteredLength = 0;
byte failedAttempts = 0;

bool unlocked = false;
bool lockout = false;

unsigned long lockoutStart = 0;
const unsigned long LOCKOUT_TIME = 30000UL;

unsigned long lastStatusSend = 0;
const unsigned long STATUS_INTERVAL_MS = 200; // AUTH heartbeat every 200 ms

void clearPIN() {
  enteredLength = 0;
  enteredPIN[0] = '\0';
}

void showHomeScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FloraSync AgriBot");
  lcd.setCursor(0, 1);
  lcd.print("Enter PIN: ");
  for (byte i = 0; i < enteredLength; i++) {
    lcd.print('*');
  }
}

void showLockedScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Robot LOCKED");
  lcd.setCursor(0, 1);
  lcd.print("Enter PIN: ");
}

void sendStatus() {
  securityLink.println(unlocked ? "AUTH:U" : "AUTH:L");
}

void setLocked() {
  unlocked = false;
  clearPIN();
  sendStatus();
  showLockedScreen();
  Serial.println("LOCKED");
}

void setup() {
  Serial.begin(9600);
  securityLink.begin(9600);

  lcd.init();
  lcd.backlight();

  clearPIN();
  setLocked();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FloraSync AgriBot");
  lcd.setCursor(0, 1);
  lcd.print("Security Ready");
  delay(1200);

  showHomeScreen();

  Serial.println("FloraSync AgriBot Security + I2C LCD");
  Serial.println("Enter 4-digit PIN, then #");
  Serial.println("* = Clear | A = Lock");
}

void loop() {
  unsigned long now = millis();

  // Send current lock state regularly so ESP32 detects link loss.
  if (now - lastStatusSend >= STATUS_INTERVAL_MS) {
    lastStatusSend = now;
    sendStatus();
  }

  if (lockout) {
    unsigned long elapsed = now - lockoutStart;

    if (elapsed >= LOCKOUT_TIME) {
      lockout = false;
      failedAttempts = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lockout ended");
      lcd.setCursor(0, 1);
      lcd.print("Try PIN again");
      delay(900);
      showHomeScreen();
      Serial.println("Lockout ended. Try again.");
    } else {
      static unsigned long lastCountdownUpdate = 0;
      if (now - lastCountdownUpdate >= 1000) {
        lastCountdownUpdate = now;
        int secondsLeft = (LOCKOUT_TIME - elapsed + 999) / 1000;
        lcd.setCursor(0, 0);
        lcd.print("Too many tries  ");
        lcd.setCursor(0, 1);
        lcd.print("Wait ");
        lcd.print(secondsLeft);
        lcd.print(" seconds   ");
      }
      return;
    }
  }

  char key = keypad.getKey();
  if (!key) return;

  if (key == 'A') {
    failedAttempts = 0;
    lockout = false;
    setLocked();
    Serial.println("Manual lock activated.");
    return;
  }

  if (unlocked) {
    // Press A at any time to lock again.
    return;
  }

  if (key == '*') {
    clearPIN();
    showHomeScreen();
    Serial.println("Entry cleared.");
    return;
  }

  if (key == '#') {
    if (enteredLength != PIN_LENGTH) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter 4 digits");
      lcd.setCursor(0, 1);
      lcd.print("Try again");
      Serial.println("Enter exactly 4 digits.");
      delay(900);
      clearPIN();
      showHomeScreen();
      return;
    }

    if (strcmp(enteredPIN, CORRECT_PIN) == 0) {
      unlocked = true;
      failedAttempts = 0;
      clearPIN();
      sendStatus();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ACCESS GRANTED");
      lcd.setCursor(0, 1);
      lcd.print("Robot Unlocked");

      Serial.println("ACCESS GRANTED - UNLOCKED");
    } else {
      failedAttempts++;
      clearPIN();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ACCESS DENIED");
      lcd.setCursor(0, 1);
      lcd.print("Attempts: ");
      lcd.print(failedAttempts);

      Serial.println("ACCESS DENIED");
      delay(1100);

      if (failedAttempts >= 3) {
        lockout = true;
        lockoutStart = millis();
        setLocked();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Too many tries");
        lcd.setCursor(0, 1);
        lcd.print("Wait 30 seconds");
        Serial.println("3 failures. Locked for 30 seconds.");
      } else {
        showHomeScreen();
      }
    }
    return;
  }

  if (key >= '0' && key <= '9' && enteredLength < PIN_LENGTH) {
    enteredPIN[enteredLength++] = key;
    enteredPIN[enteredLength] = '\0';

    lcd.setCursor(11, 1);
    for (byte i = 0; i < enteredLength; i++) {
      lcd.print('*');
    }
    for (byte i = enteredLength; i < PIN_LENGTH; i++) {
      lcd.print(' ');
    }

    Serial.println("Digit entered.");
  }
}
