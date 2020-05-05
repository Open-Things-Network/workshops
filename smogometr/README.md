# smogometr

Materiały z warsztatów, podczas których pokazaliśmy jak zbudować amatorski czujnik jakości powietrza z interfejsem LoRaWAN, podłączyć go do sieci The Things Network i przygotować prostą wizualizację przesyłanych danych na platformie Signomix. Do repozytorium dołączona jest również obszerna prezentacja z tego wydarzenia.

Warsztaty odbyły się w czerwcu 2019 roku na Uniwersytecie Kazimierza Wielkiego w Bydgoszczy (szczegóły dotyczące wydarzenia można znaleźć [tutaj](https://www.ukw.edu.pl/jednostka/del_instytut_mechaniki_i_informatyki_stosowanej/aktualnosci/39619)).

# Wymagania

## Wymagania sprzętowe

* Moduł LoRa32u4II (mikrokontroler Atmega 32U4 i układ SX1276)
* Czujnik pyłu zawieszonego SDS011
* Czujnik temperatury, wilgotności i ciśnienia BME280
* Antena 868MHz ze złączem U.FL
* Kabel micro-USB
* Przewody typu Dupont z końcówkami żeńskimi o długości ok. 10 cm
* Komputer z systemem Windows 7 lub wyższym, Mac OS X lub Linux
* Dostęp do internetu i zasięg sieci The Things Network

## Wymagania związane z oprogramowaniem
* Najnowsza wersja [Arduino IDE](https://www.arduino.cc/en/main/software)
* [Arduino-LMIC Library](https://github.com/matthijskooijman/arduino-lmic)
* [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library)
* [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor)
* [Cayenne LPP](https://github.com/ElectronicCats/CayenneLPP) lub [The Things Network Arduino Library](https://github.com/TheThingsNetwork/arduino-device-lib), która zawiera CayenneLPP

# Instalacja

1. Podłącz moduł LoRa32u4II i zainstaluj sterowniki (dotyczy tylko Windows). 
> W przypadku Windows 7 należy użyć [poprawionego sterownika](hardware/w7driver.zip).
2. Zainstaluj Arduino IDE i wszystkie wymienione wyżej biblioteki.
3. Ściągnij I rozpakuj [ten](hardware/BSFrance.zip) plik do folderu Arduino/hardware. Pozwoli to na używanie płytki LoRa32u4II w środowisku Arduino.
4. Uruchom Arduino IDE, wybierz płytkę LoRa32u4II oraz odpowiedni port szeregowy.
5. Otwórz projekt `smogometr.ino`.
6. Postępuj zgodnie z instrukcjami w załączonej prezentacji.

# Kompensacja wpływu wilgotności

Obserwując dane przesyłane przez urządzenie można zauważyć, że wskazania czujnika pyłu (wartości PM2,5 i PM10) zmieniają się w zależności od zmian wilgotności. Wpływ wilgotności na pomiary stężenia pyłu można skompensować za pomocą rozwiązań programowych, które są dość dobrze opisane przez [Opendata Stuttgart](https://github.com/opendata-stuttgart/meta/wiki/EN-Correction-for-humidity).

Drugi projekt (`smogometr_heca.ino`) dołączony do tego repozytorium jest rozszerzeniem projektu użytego podczas warsztatów i umożliwia wykorzystanie prostego rozwiązania sprzętowego w postaci [zestawu HECA](https://nettigo.pl/products/nettigo-air-monitor-heca-kit-komora-grzalki-ptc-z-modulem-sterujacym) (Heating Element Control Assembly). HECA ogrzewa powietrze zanim dotrze ono do czujnika pyłu. W ten sposób wysoka wilgotność nie zawyża pomiarów.
