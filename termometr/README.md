# termometr

Czujnik temperatury z interfejsem LoRaWAN.

# Wymagania

## Wymagania sprzętowe

* Moduł LoRa32u4II (mikrokontroler Atmega 32U4 i układ SX1276)
* Cyfrowy czujnik temperatury DS18B20 z interfejsem 1-wire
* Antena 868MHz ze złączem U.FL
* Kabel micro-USB
* Przewody połączeniowe do płytek stykowych (typu Dupont) z końcówkami żeńskimi
* Komputer z systemem Windows 7 lub wyższym, Mac OS X lub Linux
* Dostęp do internetu i zasięg sieci The Things Network

## Wymagania związane z oprogramowaniem
- Najnowsza wersja [Arduino IDE](https://www.arduino.cc/en/main/software)
- Zainstalowane biblioteki:
  - [MCCI LoRaWAN LMIC Library](https://github.com/mcci-catena/arduino-lmic)
  - [Arduino Library for Dallas Temperature ICs](https://github.com/milesburton/Arduino-Temperature-Control-Library)
  - [Low Power Library for Arduino](https://github.com/rocketscream/Low-Power)
  - [Cayenne LPP](https://github.com/ElectronicCats/CayenneLPP) lub [The Things Network Arduino Library](https://github.com/TheThingsNetwork/arduino-device-lib), która zawiera CayenneLPP

# Instalacja

1. Podłącz moduł LoRa32u4II i zainstaluj sterowniki (dotyczy tylko Windows). 
> W przypadku Windows 7 należy użyć [poprawionego sterownika](smogometr/hardware/w7driver.zip).
2. Zainstaluj Arduino IDE i wszystkie wymienione wyżej biblioteki.
3. Ściągnij i rozpakuj [ten](smogometr/hardware/BSFrance.zip) plik do folderu Arduino/hardware. Pozwoli to na używanie płytki LoRa32u4II w środowisku Arduino.
4. Uruchom Arduino IDE, wybierz płytkę LoRa32u4II oraz odpowiedni port szeregowy.
5. Konfiguracja LMIC

Przejdź do pliku `lmic_project_config.h` w bibliotece LMIC Arduino (`libraries/MCCI_LoRaWAN_LMIC_library/project_config`) i odkomentuj właściwy region - pozostałe definicje nie powinny być zmieniane:
```c
// project-specific definitions
#define CFG_eu868 1
//#define CFG_us915 1
//#define CFG_au915 1
//#define CFG_as923 1
// #define LMIC_COUNTRY_CODE LMIC_COUNTRY_CODE_JP      /* for as923-JP; also define CFG_as923 */
//#define CFG_kr920 1
//#define CFG_in866 1
#define CFG_sx1276_radio 1
//#define LMIC_USE_INTERRUPTS
```
Zmiany w pliku `libraries/MCCI_LoRaWAN_LMIC_library/src/lmic/config.h` pozwolą na odchudzenie kodu i dzięki temu nie zostanie przekroczony rozmiar dostępnej pamięci programu:
```c
...
// define this in lmic_project_config.h to disable all code related to joining
//#define DISABLE_JOIN
// define this in lmic_project_config.h to disable all code related to ping
#define DISABLE_PING
// define this in lmic_project_config.h to disable all code related to beacon tracking.
// Requires ping to be disabled too
#define DISABLE_BEACONS
...
```
6. Zarejestruj urządzenie końcowe w sieci TTN według opisu w [dokumentacji](https://www.thethingsindustries.com/docs/devices/adding-devices/#otaa-devices).
7. Otwórz projekt `termometr-otaa.ino`.
8. Wprowadź wartości `AppEUI`, `DevEUI` oraz `AppKey` uzyskane podczas rejestracji urządzenia zwracając uwagę na wymaganą kolejność bajtów.
