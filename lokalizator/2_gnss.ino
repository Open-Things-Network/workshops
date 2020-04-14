#include <TinyGPS++.h>
TinyGPSPlus gps;

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
/*
  // reset modułu GNSS (dotyczy uBlox)
  byte reset[26] = { 0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x87, 0x01, 0x00, 0x95, 0xF7 };
  Serial1.write(reset, sizeof(reset));
*/
/*  
  // wystarczy ramka NMEA GPGGA (dotyczy uBlox)
  Serial1.print("$PUBX,40,GLL,0,0,0,0*5C\r\n");
  Serial1.print("$PUBX,40,ZDA,0,0,0,0*44\r\n");
  Serial1.print("$PUBX,40,VTG,0,0,0,0*5E\r\n");
  Serial1.print("$PUBX,40,GSV,0,0,0,0*59\r\n");
  Serial1.print("$PUBX,40,GSA,0,0,0,0*4E\r\n");
  Serial1.print("$PUBX,40,RMC,0,0,0,0*47\r\n");
*/
}

void loop() 
{
  if (Serial1.available()) 
  {
    Serial.write(Serial1.read()); 
    /*
    if (gps.encode(Serial1.read()))
    {
      Serial.println("___");
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
      if (gps.time.isValid() && gps.time.isUpdated())
      {
        if (gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10) Serial.print(F("0"));
        Serial.println(gps.time.centisecond());
      }
      if (gps.location.isValid() && gps.location.isUpdated())
      {
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);
      }
      if (gps.altitude.isValid() && gps.altitude.isUpdated())
      {
        Serial.print("Altitude: ");
        Serial.println(gps.altitude.meters(), 1);
      }
      if (gps.hdop.isValid() && gps.hdop.isUpdated())
      {
        Serial.print("HDOP: ");
        Serial.println(gps.hdop.hdop());
      }
    }
    */
  }
}
