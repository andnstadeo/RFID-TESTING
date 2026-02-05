#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define GREEN_LED 2
#define RED_LED 3
#define BUZZER 4

MFRC522 rfid(SS_PIN, RST_PIN);

byte masterCard[] = {0x00, 0x00, 0x00, 0x00}; // REPLACE THIS with your Card UID

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println("System Active. Scan your card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  bool isAuthorized = true;
  Serial.print("Scanned UID:");

  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    
    // Check scanned byte against masterCard byte
    if (rfid.uid.uidByte[i] != masterCard[i]) {
      isAuthorized = false;
    }
  }
  Serial.println();

  if (isAuthorized) {
    handleSuccess();
  } else {
    handleFailure();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void handleSuccess() {
  Serial.println("Result: MATCH FOUND");
  
  // Light and Beep TOGETHER
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(300); // Short beep/light duration      NOTE: 300 = 0.3s adjust if masyadong mabilis
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

void handleFailure() {
  Serial.println("Result: MATCH FAILED");
  
  // Light and LONG Beep
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(1200); // Long duration for error       NOTE: 1200 = 1.2s adjust the value na lang ulit
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}