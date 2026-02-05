#include <SPI.h> // FOR RFID
#include <MFRC522.h>
#include <Wire.h> // I2C LCD
#include <LiquidCrystal_I2C.h>

// Pins for RFID
#define SS_PIN 10
#define RST_PIN 9

// Pins for Feedback Components
#define GREEN_LED 2
#define RED_LED 3
#define BUZZER 4

// Initialize LCD (Address 0x27 or 0x3F are common, 16x2 display)
LiquidCrystal_I2C lcd(0x27, 16, 2); 
MFRC522 rfid(SS_PIN, RST_PIN);

byte masterCard[] = {0x00, 0x00, 0x00, 0x00}; // REPLACE THIS with your Card UID

void setup() {
  Serial.begin(9600);
  SPI.begin();           
  rfid.PCD_Init();       

  lcd.init();
  lcd.backlight();
  showStandbyMessage();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  bool isAuthorized = true;

  // Compare UID with masterCard
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != masterCard[i]) {
      isAuthorized = false;
      break;
    }
  }

  if (isAuthorized) {
    accessGranted();
  } else {
    accessDenied();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  showStandbyMessage();
}

void showStandbyMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  READY TO SCAN ");
  lcd.setCursor(0, 1);
  lcd.print("  PLACE CARD... ");
}

void accessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ACCESS GRANTED ");
  lcd.setCursor(0, 1);
  lcd.print("    WELCOME!    ");
  
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(2000);
  digitalWrite(GREEN_LED, LOW);
}

void accessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ACCESS DENIED! ");
  lcd.setCursor(0, 1);
  lcd.print(" INVALID CARD   ");

  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH); delay(150); digitalWrite(BUZZER, LOW);
  delay(100);
  digitalWrite(BUZZER, HIGH); delay(150); digitalWrite(BUZZER, LOW);
  delay(2000);
  digitalWrite(RED_LED, LOW);
}