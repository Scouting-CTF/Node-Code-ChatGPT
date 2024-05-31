#define version 3

/*                         
 * Setup Receiving Server
 */
const char* server  = "vuurmuur.west-data.nl";
const int  port       = 8990;

/*  
 * Setup APN
 */
const char apn[]      = "internet"; // APN
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password
const char simPIN[]   = ""; //SIM Pin

/*
* Setup Status LEDS
*/
#define LED_RED_GPIO 32 
#define LED_BLUE_GPIO 33
#define LED_GREEN_GPIO 34

#define LED_RED_ON HIGH
#define LED_RED_OFF LOW
#define LED_BLUE_ON HIGH
#define LED_BLUE_OFF LOW
#define LED_GREEN_ON HIGH
#define LED_GREEN_OFF LOW

#define SIM800L_IP5306_VERSION_20200811

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800          // Modem is SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set RX buffer to 1Kb


// Libraries to include
#include "utilities.h"
#include <Wire.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <axp20x.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 21
#define RST_PIN 5
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

// Init array that will store new NUID 
byte nuidPICC[4];

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


// TinyGSM Client for Internet connection
TinyGsmClient client(modem);

HttpClient http = HttpClient(client, server, port);

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */


//bool Alarm;
//int alarmInputState;

RTC_DATA_ATTR int bootCount = 0;  // Variable in RTC memory

String PostImei;
String PostSystemID;
String PostNodeID;
String PostAlarm;
String modemInfo;
String uuid = "";
String PostUUID;
void setupModem()
{
#ifdef MODEM_RST
  // Keep reset high
  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);
#endif

  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  // Turn on the Modem power first
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Pull down PWRKEY for more than 1 second according to manual requirements
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);
}


void turnOffNetlight()
{
  SerialMon.println("Turning off SIM800 Red LED...");
  modem.sendAT("+CNETLIGHT=0");
}


void turnOnNetlight()
{
  SerialMon.println("Turning on SIM800 Red LED...");
  modem.sendAT("+CNETLIGHT=1");
}

void printUUID(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
      uuid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      uuid += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("Card UID: " + uuid);
}

void uploadUUID(){
  // Start modem
  Serial.println("Setting up onboard modem module....");
  setupModem();

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
 
  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork(240000L)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  // Start GPRS Verbinding
  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");
  http.connectionKeepAlive();  // Currently, this is needed for HTTPS

//// Hier tussen moet de POST naar het internet met de UUID data
Serial.println(F("Data uploaded"));


Serial.println("making POST request");

  PostSystemID = "SystemID=CTF&";
  PostNodeID = "NodeID=Echo";
 //PostImei = modem.getIMEI();
  PostUUID = "&UUID=" + String(uuid);
  String postData = PostSystemID + PostNodeID + PostImei + PostUUID;

  Serial.println(postData);
  http.beginRequest();
  http.post("/ctf");
  http.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  http.sendHeader("Content-Length", postData.length());
  http.beginBody();
  http.print(postData);
  http.endRequest();

  // read the status code and body of the response
  int statusCode = http.responseStatusCode();

  Serial.print("Status code: ");
  Serial.println(statusCode);








  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));
  turnOffNetlight();

digitalWrite(LED_GPIO, LED_OFF);
}

void setup() {
  SerialMon.begin(115200);
  delay(2);
  // Start power management
  if (setupPMU() == false) {
    Serial.println("Setting power error");
  }
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  Serial.println(F("System booted and ready for usage!"));
  // Initialize the indicator as an output
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LED_ON);
}  //End Setup

void loop() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    
    printUUID(rfid.uid.uidByte, rfid.uid.size); //Print Card UUID from string.
    Serial.println();
//Working loop for storing code for upload UUID

    uploadUUID();

  }
  else Serial.println(F("Card Already Scanned!"));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}