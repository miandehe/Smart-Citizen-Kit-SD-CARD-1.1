#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "Constants.h"
#include "SCKDrivers.h"

SCKDriver driver;

#define USBEnabled      true 
#define sensorEnabled   true
#define debuggEnabled   false

uint32_t timetransmit = 0;  

File myFile;

float SENSORvalue[13];

void setup() {
  sckBegin();
  sckAccelDefault();
  sckConfig();
  driver.WIFIoff();
  #if debuggEnabled
    Serial.print(F("Initializing SD card..."));
  #endif 
    if (!SD.begin(CS_MMC)) {
  #if debuggEnabled
      Serial.println(F("initialization failed!"));
  #endif 
      return;
    }
  #if debuggEnabled
    Serial.println(F("initialization done."));   
  #endif 
    if (!SD.exists("post.csv")) {
      #if debuggEnabled
        Serial.println(F("Creating post.csv..."));
      #endif 
      SD.open("post.csv", FILE_WRITE);
      myFile.close();
      delay(1000);
      txHeader();
      
    } else{
      #if debuggEnabled
        Serial.println(F("post.csv exists ..."));
      #endif 
    }
    timetransmit = millis();
}

void loop() {  
  #if sensorEnabled  
    if ((millis()-timetransmit) >= (unsigned long)TIME_UPDATE*1000)
    {  
      Serial.println("*** loop ***");
      timetransmit = millis();
      updateSensorsSD();
      txSD();
      #if USBEnabled
          txDebugSD();
      #endif
    }
  #endif
}
