RFID-ESP32-GPRS-LCD Project

Dit project demonstreert een ESP32-gebaseerde oplossing met de mogelijkheid om RFID-kaarten te lezen, de gegevens via GPRS naar een server te sturen en de status weer te geven op een I2C LCD-scherm.

Benodigdheden:

LilyGO T-Call ESP32 bord met LTE/GPRS module
MFRC522 RFID-lezer
I2C LCD-scherm (2x16 karakters)
RFID-kaarten (Voorgeprogrammeerd met unieke ID's)
Aansluitingen:

Sluit de MFRC522 RFID-lezer aan op de ESP32 volgens de specificaties van de bibliotheek die wordt gebruikt.
Verbind het I2C LCD-scherm met de SDA- en SCL-pinnen van de ESP32.
Code:

Upload de gegeven Arduino-code naar de ESP32.
Werkingsprincipe:

Bij opstarten toont het LCD-scherm "Booting.." en maakt verbinding met het GPRS-netwerk.
Zodra de verbinding is gemaakt, wordt "Ready" weergegeven op het scherm en de groene LED gaat branden.
Wanneer een RFID-kaart wordt gedetecteerd, wordt de unieke ID van de kaart gelezen en gecontroleerd tegen een lijst van bekende kaarten.
Als de kaart bekend is, wordt de ID via een HTTP POST-verzoek naar de server verzonden.
Het LCD-scherm toont "Captured" en de rode LED gaat branden.
Na de succesvolle verzending wordt nieuwe RFID-invoer niet meer geaccepteerd.
Het apparaat gaat vervolgens in een korte slaapstand voordat het de lus opnieuw start.
Opmerkingen:

De deep sleep-functie is uitgeschakeld in deze code.
De debug-modus is uitgeschakeld.
Controleer of alle bibliotheken correct zijn geïnstalleerd.
Notities:

Dit project kan worden aangepast om verschillende taken uit te voeren of andere sensoren toe te voegen, afhankelijk van de behoeften van het specifieke project.
Zorg ervoor dat de nodige beveiligingsmaatregelen worden genomen bij het verzenden van gevoelige gegevens via GPRS. Gebruik bijvoorbeeld HTTPS en overweeg het gebruik van API-sleutels of andere authenticatiemethoden.
Credits:

Deze code is ontwikkeld met behulp van Arduino IDE en verschillende libraries zoals MFRC522, TinyGSM en ArduinoHttpClient.
Licentie:

Deze code is beschikbaar onder de MIT-licentie. Zie de LICENSE voor meer informatie.
Voor eventuele vragen of ondersteuning, neem contact op met de ontwikkelaar.
