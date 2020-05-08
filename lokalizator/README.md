# lokalizator
Celem warsztatów było przybliżenie technologii LoRaWAN oraz systemów nawigacji satelitarnej. Poruszone były również tematy związane z możliwością wykorzystania LoRaWAN do lokalizacji obiektów oraz przesyłania danych z balonów stratosferycznych. 

Podczas zajęć budowaliśmy prosty lokalizator wykorzystujący system GPS do określenia pozycji urządzenia. Dane przesyłane przez urządzenie za pośrednictwem sieci The Things Network były wizualizowane na platformie Signomix.  

Materiały te zostały przygotowane na potrzeby warsztatów, które odbyły się w październiku 2019 roku w ramach wydarzenia [World Space Week](http://worldspaceweek.pl) we Wrocławiu.

# Wymagania

## Wymagania sprzętowe

* Moduł LoRa32u4II (mikrokontroler Atmega 32U4 i układ SX1276)
* Odbiornik GPS z interfejsem szeregowym i anteną (np. popularny moduł z układem u-blox NEO-6M)
* Antena 868MHz ze złączem U.FL
* Kabel micro-USB
* Przewody typu Dupont z końcówkami żeńskimi o długości ok. 10 cm
* Komputer z systemem Windows 7 lub wyższym, Mac OS X lub Linux
* Dostęp do internetu i zasięg sieci The Things Network

## Wymagania związane z oprogramowaniem
- Najnowsza wersja [Arduino IDE](https://www.arduino.cc/en/main/software)
- Zainstalowane biblioteki:
  - [Arduino-LMIC Library](https://github.com/matthijskooijman/arduino-lmic)
  - [TinyGPSPlus - NMEA Parsing Library](https://github.com/mikalhart/TinyGPSPlus)
  - [Cayenne LPP](https://github.com/ElectronicCats/CayenneLPP) lub [The Things Network Arduino Library](https://github.com/TheThingsNetwork/arduino-device-lib), która zawiera CayenneLPP

# Instalacja

1. Podłącz moduł LoRa32u4II i zainstaluj sterowniki (dotyczy tylko Windows). 
> W przypadku Windows 7 należy użyć [poprawionego sterownika](hardware/w7driver.zip).
2. Zainstaluj Arduino IDE i wszystkie wymienione wyżej biblioteki.
3. Ściągnij i rozpakuj [ten](hardware/BSFrance.zip) plik do folderu Arduino/hardware. Pozwoli to na używanie płytki LoRa32u4II w środowisku Arduino.
4. Uruchom Arduino IDE, wybierz płytkę LoRa32u4II oraz odpowiedni port szeregowy.
5. Otwórz wybrany projekt.
6. Postępuj zgodnie z instrukcjami w załączonej prezentacji.

# Załączone projekty
* `1_lorawan_abp.ino` - przesyłanie danych przez sieć LoRaWAN 
* `2_gnss.ino` - testowanie komunikacji z modułem GPS
* `3_lokalizator.ino` - ostateczny projekt użyty do uruchomienia lokalizatora

Pliki `1_payload_decoder.js` oraz `3_payload_decoder.js` zawierają skrypty dekodujące dane przesyłane przez urządzenie w projektach 1 i 3.
