#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define GREEN_LED 2
#define RED_LED 3
#define BUZZER 4

LiquidCrystal_I2C lcd(0x27, 16, 2); 
MFRC522 rfid(SS_PIN, RST_PIN)

// Replace these with your actual Master Card UID bytes
byte masterCard[] = {0x00, 0x00, 0x00, 0x00}; 

void setup() {
  Serial.begin(9600);
  SPI.begin();           
  rfid.PCD_Init();       

  lcd.init();
  lcd.backlight();
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println(F("--- Access Control System Ready ---"));
  showStandbyMessage();
}

void loop() {
  // 1. Look for cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // 2. Debug: Print the Scanned UID to Serial Monitor
  Serial.print(F("Scanned UID:"));
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // 3. Comparison Logic
  if (isMaster(rfid.uid.uidByte)) {
    handleAccess(true);
  } else {
    handleAccess(false);
  }

  // 4. Cleanup & Reset
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(500); // Short delay to prevent accidental double-scans
  showStandbyMessage();
}


bool isMaster(byte scanned[]) {
  for (byte i = 0; i < 4; i++) {
    if (scanned[i] != masterCard[i]) return false;
  }
  return true;
}

void handleAccess(bool granted) {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (granted) {
    Serial.println(F("Result: ACCESS GRANTED"));
    lcd.print(" ACCESS GRANTED ");
    lcd.setCursor(0, 1);
    lcd.print("    WELCOME!    ");
    
    digitalWrite(GREEN_LED, HIGH);
    tone(BUZZER, 2500, 200); // High-pitched beep
    delay(2000);
    digitalWrite(GREEN_LED, LOW);
  } else {
    Serial.println(F("Result: ACCESS DENIED"));
    lcd.print(" ACCESS DENIED! ");
    lcd.setCursor(0, 1);
    lcd.print(" INVALID CARD   ");
    
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 800, 150); // Low-pitched warning
    delay(200);
    tone(BUZZER, 800, 150);
    delay(1800);
    digitalWrite(RED_LED, LOW);
  }
}

void showStandbyMessage() {
  lcd.clear();
  lcd.print("  READY TO SCAN ");
  lcd.setCursor(0, 1);
  lcd.print("  PLACE CARD... ");
}
