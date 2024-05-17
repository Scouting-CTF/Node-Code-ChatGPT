#include <Wire.h>
#include <MFRC522.h>
//#include <TinyGsmClient.h>
//#include <ArduinoHttpClient.h>

#define RST_PIN         9           // Defineer de RST_PIN voor MFRC522
#define SS_PIN          10          // Defineer de SS_PIN voor MFRC522
#define GREEN_LED_PIN   2           // Defineer de pin voor de groene LED
#define RED_LED_PIN     3           // Defineer de pin voor de rode LED

#define SERVER_ADDRESS  "http://your-server.com/endpoint"  // Voeg hier je serveradres toe

//#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_RX      2
//#define TINY_GSM_TX      3

const char apn[] = "your_apn";  // Voeg hier je APN toe
const char gprsUser[] = "your_user";  // Voeg hier je GPRS gebruikersnaam toe
const char gprsPass[] = "your_password";  // Voeg hier je GPRS wachtwoord toe

//TinyGsm modem(TinyGsmSerial(TinyGsmClient(TINY_GSM_TX, TINY_GSM_RX)));
MFRC522 mfrc522(SS_PIN, RST_PIN);         // Creëer MFRC522-instance

bool isRfidAccepted = true;

void setup() {
  Serial.begin(115200);

  SPI.begin();          
  mfrc522.PCD_Init();  // Initieer MFRC522

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  digitalWrite(GREEN_LED_PIN, HIGH); // Groene LED aan
}

void loop() {
  // Ontwaak de ESP

  if (isRfidAccepted && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();

    if (cardUID == "11223344" || cardUID == "55667788" || cardUID == "AABBCCDD" ||
        cardUID == "EEFF0011" || cardUID == "12345678" || cardUID == "90ABCDEF") {
      // Maak een HTTP POST-verzoek naar de server
      String payload = "rfid_token=" + cardUID;
      serial.print(payload);
      isRfidAccepted = false; // Schakel RFID-acceptatie uit na succesvolle HTTP POST
    }

    delay(100);  // Even wachten om te voorkomen dat de kaart direct opnieuw wordt gedetecteerd
  }

  digitalWrite(GREEN_LED_PIN, LOW);  // Groene LED uit
  digitalWrite(RED_LED_PIN, HIGH);   // Rode LED aan

  delay(1000);  // Korte wachttijd voordat de lus opnieuw wordt uitgevoerd
}