#include <lmic.h>
#include <hal/hal.h>
#include <sds.h>
#include <Adafruit_BME280.h>
#include <ClosedCube_SHT31D.h>
#include <CayenneLPP.h>

// devID: ..., appID: ...
static const u1_t PROGMEM NWKSKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u1_t PROGMEM APPSKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u4_t DEVADDR = 0x00000000;

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

const lmic_pinmap lmic_pins = {
    .nss = 8,                      
    .rxtx = LMIC_UNUSED_PIN,       
    .rst = 4,                     
    .dio = { 7, 6, LMIC_UNUSED_PIN },
};

// czujnik pyłu SDS011 (UART)
SDS sds(Serial1);
SDS::DATA data;

// czujnik BME280 (I2C)
Adafruit_BME280 bme;

// HECA (Heating Element Control Assembly - I2C)
ClosedCube_SHT31D heca;

// zadania
static osjob_t sendjob; // wysyłanie danych 
static osjob_t readjob; // odczyt czujników

// interwał odczytu w sekundach
const unsigned READ_INTERVAL = 60;

// bufor danych
CayenneLPP lpp(51);


void onEvent (ev_t ev) 
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch(ev) 
  {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
      {
        Serial.println(F("Received ack"));
      }
      if (LMIC.dataLen) 
      {
        // data received in rx slot after tx
        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        Serial.println();
      }
      os_setTimedCallback(&readjob, os_getTime() + sec2osticks(READ_INTERVAL), do_read);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
     default:
      Serial.println(F("Unknown event"));
      break;
  }
}

void do_send(osjob_t* j)
{
  if (LMIC.opmode & OP_TXRXPEND) 
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } 
  else 
  { 
    LMIC_setTxData2(2, lpp.getBuffer(), lpp.getSize(), 0);
    Serial.println(F("Packet queued"));
  }
}

void do_read(osjob_t* j) 
{
  // uruchomienie czujnika pyłu z oczekiwaniem 30s na rozruch
  Serial.println("SDS wakeup");
  sds.wakeUp();
  delay(30000); 

  // odczyt czujnika BME280
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F; // hPa
  float humidity = bme.readHumidity();
  Serial.print("Temperature (°C): ");
  Serial.println(temperature);
  Serial.print("Pressure (hPa): ");
  Serial.println(pressure);
  Serial.print("Humidity (%): ");
  Serial.println(humidity);

  // odczyt HECA
  bool heca_ok = false;
  float heca_temperature = 0;
  float heca_humidity = 0;
  SHT31D result = heca.periodicFetchData();
  if (result.error == SHT3XD_NO_ERROR)
  {
    heca_temperature = result.t;
    heca_humidity = result.rh;
    Serial.print("HECA temperature (°C): ");
    Serial.println(heca_temperature);
    Serial.print("HECA humidity (%): ");
    Serial.println(heca_humidity);
    heca_ok = true;
  }
  
  // odczyt czujnika pyłu SDS011
  bool sds_ok = false;
  sds.requestRead();
  if (sds.readUntil(data))
  {
    Serial.print("PM 2.5 (µg/m³): ");
    Serial.println(data.pm_25 / 10.0);
    Serial.print("PM 10.0 (µg/m³): ");
    Serial.println(data.pm_100 / 10.0);
    sds_ok = true;
  }
  
  // uśpienie czujnika pyłu
  Serial.println("SDS sleep");
  sds.sleep();
  
  // przygotowanie danych
  lpp.reset();
  lpp.addTemperature(1, temperature);
  lpp.addRelativeHumidity(2, humidity);
  lpp.addBarometricPressure(3, pressure);
  if (sds_ok) 
  {
    lpp.addAnalogInput(4, data.pm_25 / 10.0);
    lpp.addAnalogInput(5, data.pm_100 / 10.0);
  }
  if (heca_ok)
  {
    lpp.addTemperature(6, heca_temperature);
    lpp.addRelativeHumidity(7, heca_humidity);
  }
  
  // wysyłanie danych
  do_send(&sendjob);
}

void setup() 
{
  Serial.begin(9600);
  
  Serial1.begin(9600);
  sds.passiveMode();

  bme.begin(0x76);
  
  heca.begin(0x44);
  if (heca.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_1HZ) == SHT3XD_NO_ERROR) 
  {
    heca.writeAlertHigh(120, 119, 63, 60); 
    heca.writeAlertLow(-5, 5, 0, 1);
    heca.clearAll(); 
  }

  os_init();
  LMIC_reset();
  #ifdef PROGMEM
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
  #else
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
  #endif
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);    
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);  
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI); 
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI); 
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI); 
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7, 14);
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  LMIC.dn2Dr = DR_SF9;
  
  do_read(&readjob);
}

void loop() 
{
  os_runloop_once(); 
}
