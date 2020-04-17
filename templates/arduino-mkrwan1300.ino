// Better project template for Arduino MKRWAN 1300/1310 board with SAMD21 microcontroller and Murata CMWX1ZZABZ LoRa® module
#include <MKRWAN.h>

const unsigned long sendInterval = 60000; // send interval in ms
unsigned long lastSendTime = 0;

LoRaModem modem;

// OTAA - set your AppEUI and AppKey
String appEui = "0000000000000000"; 
String appKey = "00000000000000000000000000000000";

// ABP - set your DevAddr and session keys
//String devAddr;
//String nwkSKey;
//String appSKey;

void setup() 
{
  Serial.begin(115200);
  
  if (!modem.begin(EU868)) 
  {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Module version: ");
  Serial.println(modem.version());
  Serial.print("Device EUI: ");
  Serial.println(modem.deviceEUI());

  // OTAA
  int connected = modem.joinOTAA(appEui, appKey);
  // ABP
  //int connected = modem.joinABP(devAddr, nwkSKey, appSKey);

  if (!connected) 
  {
    Serial.println("Not connected");
    while (1) {}
  }

  // set DataRate - DR0..DR5 (SF12..SF7)
  modem.dataRate(5); 
  // set transceiver power - RFO or PABOOST, second parameter 0..5 (see SX127X datasheet)
  modem.power(RFO, 5);
  // respect recommendations related to bandwidth usage (Duty Cycle)
  modem.dutyCycle(true);    
  // set device class
  modem.configureClass(CLASS_A);
  // optional heartbeat message interval in seconds
  //modem.pollInterval(300);
  
  lastSendTime = millis();
}

void loop() 
{ 
  // send message
  if (millis() - lastSendTime >= sendInterval)
  {
    lastSendTime = millis();
    
    // set FPort
    modem.setPort(3);

    // initialize message
    modem.beginPacket();

    // set data format - true (HEX string) or false (string)
    modem.format(false);
    modem.print("Hello");
    //modem.format(true);
    //modem.print("31323334353637");
    
    // send message - false (unconfirmed), true (confirmed)
    int err = modem.endPacket(false); 
    if (err > 0) 
    {
      Serial.println("Message sent");
    } 
    else 
    {
      Serial.println("Error sending message");
    }
  }

  // receive message
  if (modem.parsePacket())
  {
    Serial.write(modem.read());
    Serial.println();
  }

  // optional heartbeat message - payload 0x00, confirmed
  //modem.poll(); 
}