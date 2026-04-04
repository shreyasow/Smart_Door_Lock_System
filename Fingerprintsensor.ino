#include <Wire.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ---------------- LCD CONNECTIONS ----------------
#define LCD_RS 5
#define LCD_EN 18
#define LCD_D4 19
#define LCD_D5 21
#define LCD_D6 4
#define LCD_D7 2
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// ---------------- SERVO & RELAY ----------------
Servo doorServo;
#define SERVO_PIN 22
#define RELAY_PIN 14

// ---------------- FINGERPRINT ----------------
HardwareSerial mySerial(1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// ---------------- WIFI + GOOGLE SHEET ----------------
const char* ssid = "Saraswathy";
const char* password = "SARA2007";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbwRmWwneyNE-zUSmDz07gyyawHnWe7RIsrEVQHoZ0bY6iWIOrshoBGZuHEUIIcihLgM/exec";

// ---------------- FUNCTION DECLARATIONS ----------------
void enrollFingerprint(uint8_t id);
int getFingerprintID();
void logToSheet(int fid, String result, String msg);

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("System Starting...");
  Serial.println("System Starting...");
  delay(2000);

  // Servo setup
  doorServo.attach(SERVO_PIN);
  doorServo.write(0);

  // Relay setup
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Fingerprint setup
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  lcd.clear();
  lcd.print("Checking Sensor...");
  Serial.println("Checking Fingerprint Sensor...");
  delay(1000);

  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("FP Sensor Ready");
    Serial.println("Fingerprint Sensor Ready");
  } else {
    lcd.clear();
    lcd.print("Sensor Error!");
    Serial.println("Fingerprint Sensor not found. Check connections!");
    while (1) delay(1);
  }

  // Wi-Fi setup
  lcd.clear();
  lcd.print("Connecting WiFi");
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  lcd.print("WiFi Connected!");
  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(2000);

  // Enroll 3 fingerprints
  lcd.clear();
  lcd.print("Enroll 3 Fingers");
  Serial.println("Starting fingerprint enrollment for IDs 1–3...");
  for (int i = 1; i <= 3; i++) {
    enrollFingerprint(i);
    delay(1500);
  }

  lcd.clear();
  lcd.print("Ready for Use");
  Serial.println("System Ready for Use");
  delay(2000);
}

// ---------------- MAIN LOOP ----------------
void loop() {
  int id = getFingerprintID();

  if (id > 0 && id <= 3) {
    lcd.clear();
    lcd.print("Access Granted");
    Serial.println("Access Granted - Finger ID: " + String(id));
    logToSheet(id, "Granted", "Door unlocked");

    doorServo.write(180);
    digitalWrite(RELAY_PIN, HIGH);
    delay(3000);

    doorServo.write(0);
    digitalWrite(RELAY_PIN, LOW);
    lcd.clear();
    lcd.print("Door Locked");
    Serial.println("Door Locked Again");
    delay(2000);
  }
  else if (id == -1) {
    lcd.clear();
    lcd.print("No Finger Found");
    Serial.println("No Finger Found");
    delay(1000);
  }
  else {
    lcd.clear();
    lcd.print("Access Denied");
    Serial.println("Access Denied - Unrecognized Finger");
    logToSheet(0, "Denied", "Unrecognized Finger");
    delay(2000);
  }
}

// ---------------- ENROLL FUNCTION ----------------
void enrollFingerprint(uint8_t id) {
  int p = -1;
  lcd.clear();
  lcd.print("Place Finger ");
  lcd.print(id);
  Serial.println("Place finger for ID " + String(id));

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return;

  lcd.clear();
  lcd.print("Remove Finger");
  Serial.println("Remove Finger");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  lcd.clear();
  lcd.print("Place same");
  lcd.setCursor(0, 1);
  lcd.print("finger again");
  Serial.println("Place same finger again");

  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return;

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Error Matching");
    Serial.println("Error: Fingerprints did not match.");
    delay(2000);
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Stored Finger ");
    lcd.print(id);
    Serial.println("Stored Fingerprint with ID " + String(id));
  } else {
    lcd.clear();
    lcd.print("Store Failed");
    Serial.println("Failed to store fingerprint for ID " + String(id));
  }
  delay(2000);
}

// ---------------- FINGERPRINT SCAN ----------------
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return 0;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    return finger.fingerID;
  } else {
    return 0;
  }
}

// ---------------- GOOGLE SHEET LOGGING ----------------
void logToSheet(int fid, String result, String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(scriptURL) + "?fid=" + fid + "&result=" + result + "&msg=" + msg;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Data logged to Google Sheet: " + result);
    } else {
      Serial.println("Error logging data to Google Sheet.");
    }
    http.end();
  } else {
    Serial.println("Wi-Fi not connected. Could not log to Sheet.");
  }
}
