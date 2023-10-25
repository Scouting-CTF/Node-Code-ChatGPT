#include <Wire.h>
#include <MFRC522.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <LiquidCrystal_I2C.h>  // Voeg de I2C LCD-bibliotheek toe

#define RST_PIN         9           // Defineer de RST_PIN voor MFRC522
#define SS_PIN          10          // Defineer de SS_PIN voor MFRC522
#define GREEN_LED_PIN   2           // Defineer de pin voor de groene LED
#define RED_LED_PIN     3           // Defineer de pin voor de rode LED

#define SERVER_ADDRESS  "http://your-server.com/endpoint"  // Voeg hier je serveradres toe

#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX      2
#define TINY_GSM_TX      3

const char apn[] = "your_apn";  // Voeg hier je APN toe
const char gprsUser[] = "your_user";  // Voeg hier je GPRS gebruikersnaam toe
const char gprsPass[] = "your_password";  // Voeg hier je GPRS wachtwoord toe

TinyGsm modem(TinyGsmSerial(TinyGsmClient(TINY_GSM_TX, TINY_GSM_RX)));
MFRC522 mfrc522(SS_PIN, RST_PIN);         // Creëer MFRC522-instance
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adres van het I2C-scherm, 16 kolommen, 2 regels

bool isRfidAccepted = true;

void setup() {
  Serial.begin(115200);

  SPI.begin();          
  mfrc522.PCD_Init();  // Initieer MFRC522

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  lcd.begin(16,2);  // Start het LCD-scherm
  lcd.print("Booting..");  // Toon "Booting.." op het LCD-scherm

  TinyGsmAutoBaud(Serial);
  
  if (!modem.restart()) {
    Serial.println(F("Failed to restart modem"));
    while (1);
  }

  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(F("Failed to connect to GPRS network"));
    while (1);
  }
  
  lcd.clear();  // Wis het scherm
  lcd.print("Ready");  // Toon "Ready" op het LCD-scherm
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
      httpPostRequest(payload);
      isRfidAccepted = false; // Schakel RFID-acceptatie uit na succesvolle HTTP POST
      lcd.clear();  // Wis het scherm
      lcd.print("Captured");  // Toon "Captured" op het LCD-scherm
    }

    delay(100);  // Even wachten om te voorkomen dat de kaart direct opnieuw wordt gedetecteerd
  }

  digitalWrite(GREEN_LED_PIN, LOW);  // Groene LED uit
  digitalWrite(RED_LED_PIN, HIGH);   // Rode LED aan

  delay(1000);  // Korte wachttijd voordat de lus opnieuw wordt uitgevoerd
}

void httpPostRequest(String payload) {
  HttpClient client = HttpClient(modem, SERVER_ADDRESS);

  client.beginRequest();
  client.post("/endpoint");
  client.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  client.sendHeader("Content-Length", payload.length());
  client.sendHeader("Connection", "close");
  client.beginBody();
  client.sendBody(payload);
  client.endRequest();

  int status = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print(F("HTTP response code: "));
  Serial.println(status);
  Serial.print(F("Server response: "));
  Serial.println(response);
}
