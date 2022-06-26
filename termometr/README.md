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
5. Otwórz i uruchom projekt `termometr.ino`.
