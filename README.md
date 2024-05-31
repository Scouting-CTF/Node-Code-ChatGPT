RFID-ESP32-GPRS-LCD Project

Dit project demonstreert een ESP32-gebaseerde oplossing met de mogelijkheid om RFID-kaarten te lezen, de gegevens via GPRS naar een server te sturen en de status weer te geven op een I2C LCD-scherm.

Benodigdheden:

LilyGO T-Call ESP32 bord met LTE/GPRS module
MFRC522 RFID-lezer
3 kleuren leds, Groen Blauw en Rood
RFID-kaarten (Voorgeprogrammeerd met unieke ID's)
Aansluitingen:

Sluit de MFRC522 RFID-lezer aan op de ESP32 volgens de specificaties van de bibliotheek die wordt gebruikt.
Code:

Upload de gegeven Arduino-code naar de ESP32.
Werkingsprincipe:

Zodra de ESP van stroom wordt voorzien wordt de RFID reader geinitialiseerd.
Zodra dit klaar is gaat de groene led branden waarmee aangegeven wordt dat het systeem klaar is om te scannen.
Als er een tag gescand wordt gaat de groene led uit en gaat er een blauwe led aan, dit geeft aan dat de gegevens worden verstuurd.
Zodra alle gegevens succesvol zijn verstuurd gaat de blauwe led uit en de rode led aan. Dit geeft aan dat de ESP klaar is en er niks meer mee gedaan kan worden.


Opmerkingen:

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
