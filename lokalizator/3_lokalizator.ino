#include <lmic.h>
#include <hal/hal.h>
#include <CayenneLPP.h>
#include <TinyGPS++.h>

//#define TTN_MAPPER  // jesli ma być integracja z TTN Mapper

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

// odbiornik GPS
static const uint32_t gps_baud = 9600;
TinyGPSPlus gps;

// zadania
static osjob_t readjob; // odczyt z GPS
static osjob_t sendjob; // wysyłanie danych 

// interwał wysyłania danych w sekundach
const unsigned TX_INTERVAL = 15;

// bufor danych
#ifdef TTN_MAPPER
  uint8_t data[10];
#else
  CayenneLPP lpp(51);
#endif

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
        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        Serial.println();
      }
      os_setTimedCallback(&readjob, os_getTime() + sec2osticks(TX_INTERVAL), do_read);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
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
      Serial.println((unsigned) ev);
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
#ifdef TTN_MAPPER
    LMIC_setTxData2(2, data, sizeof(data), 0);
#else
    LMIC_setTxData2(2, lpp.getBuffer(), lpp.getSize(), 0);
#endif
    Serial.println(F("Packet queued"));
  }
}

void do_read(osjob_t* j) 
{
  // przygotowanie danych
  
  // HDOP (Horizontal Dilution Of Precision):
  //  0 - pomiar pozycji niemożliwy
  //  < 1 - idealny
  //  2..3 - znakomity
  //  4..6 - dobry
  //  7..8 - umiarkowany
  //  9..20 - słaby
  //  > 20 - zły
  Serial.println("GPS reading...");
  if (gps.location.isValid() && gps.location.isUpdated() && gps.altitude.isValid() && gps.altitude.isUpdated())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters(), 1);
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    if (gps.hdop.isValid())
    {
      Serial.print("HDOP: ");
      Serial.println(gps.hdop.hdop());
    }
#ifdef TTN_MAPPER
    uint32_t latitude = ((gps.location.lat() + 90) / 180.0) * 16777215;
    uint32_t longitude = ((gps.location.lng() + 180) / 360.0) * 16777215;
    uint16_t altitude = gps.altitude.meters();
    uint8_t hdev = gps.hdop.hdop() * 10;
    data[0] = latitude >> 16;
    data[1] = latitude >> 8;
    data[2] = latitude;
    data[3] = longitude >> 16;
    data[4] = longitude >> 8;
    data[5] = longitude;
    data[6] = altitude >> 8;
    data[7] = altitude;
    data[8] = hdev;
    data[9] = gps.satellites.value();
#else
    lpp.reset();
    lpp.addGPS(1, gps.location.lat(), gps.location.lng(), gps.altitude.meters());
    lpp.addAnalogInput(2, (float)gps.satellites.value());
    if (gps.hdop.isValid())
    {
      lpp.addAnalogInput(3, gps.hdop.hdop());   
    }
#endif
  }
  
  // wysyłanie danych
  do_send(&sendjob);
}

void setup() 
{
  Serial.begin(9600);
  
  Serial1.begin(gps_baud);
  delay(500);
  // wystarczy ramka NMEA GPGGA (dotyczy uBlox)
  Serial1.print("$PUBX,40,GLL,0,0,0,0*5C\r\n");
  Serial1.print("$PUBX,40,ZDA,0,0,0,0*44\r\n");
  Serial1.print("$PUBX,40,VTG,0,0,0,0*5E\r\n");
  Serial1.print("$PUBX,40,GSV,0,0,0,0*59\r\n");
  Serial1.print("$PUBX,40,GSA,0,0,0,0*4E\r\n");
  Serial1.print("$PUBX,40,RMC,0,0,0,0*47\r\n");
  
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
  while (Serial1.available()) 
  {
    gps.encode(Serial1.read());
  }
}
