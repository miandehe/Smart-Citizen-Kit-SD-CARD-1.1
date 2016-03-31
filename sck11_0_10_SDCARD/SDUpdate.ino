void txSD() {
  Serial.println("*** txSD ***");
  // if the file opened okay, write to it:
  myFile = SD.open("post.csv", FILE_WRITE);
  if (myFile) {
  #if debuggEnabled
      Serial.println(F("Writing...")); 
  #endif 
    for (int i=0; i<12; i++)
    {
      myFile.print(SENSORvalue[i]);
      myFile.print(",");
    }
    myFile.print(driver.RTCtime());
    myFile.println();
    // close the file:
    myFile.close();
#if debuggEnabled
    Serial.println(F("Closing...")); 
#endif 
  }
}

char* SENSOR[13]={
  "Temperature",
  "Humidity",
  "Light",
  "Battery",
  "Solar panel",
  "Charger current",
  "Carbon Monxide",
  "Nitrogen Dioxide",
  "Noise",
  "Accel X",
  "Accel Y",
  "Accel Z",
  "UTC"
};

char* UNITS[13]={
  " C",
  " %",
  " lx",
  " mV",
#if DataRaw
  " mV",
#else
  " %",
#endif
  " mA",
  " Ohm",
  " Ohm",
#if DataRaw
  " mV",
#else
  " dB",
#endif
  "",
  "",
  "",
  "" 
};            

void updateSensorsSD() {
    sckGetSHT(&SENSORvalue[0], &SENSORvalue[1]);
    sckGetMICS(&SENSORvalue[6], &SENSORvalue[7]);
    SENSORvalue[2] = sckGetLight(); // %
    SENSORvalue[3] = sckGetBattery(); //%
    SENSORvalue[4] = driver.panelRead(); //%
    SENSORvalue[5] = driver.readCharge();  // mA
    SENSORvalue[8] = sckGetNoise(); //dB  
    sckReadAcc(&SENSORvalue[9], &SENSORvalue[10], &SENSORvalue[11]);
}

void txDebugSD() {
  Serial.println("*** txDebugSD ***");
  for(int i=0; i<12; i++) 
  {
    Serial.print(SENSOR[i]);
    Serial.print(": "); 
    Serial.print(SENSORvalue[i]); 
    Serial.println(UNITS[i]);
  }
  Serial.print(SENSOR[12]);
  Serial.print(": "); 
  Serial.println(driver.RTCtime());
  Serial.println(F("*******************"));     
}

void txHeader() { 
  Serial.println("*** txHeader ***");
  // if the file opened okay, write to it:
  if (SD.open("post.csv", FILE_WRITE)) {
  #if debuggEnabled
      Serial.println(F("Writing...")); 
  #endif 
    for (int i=0; i<12; i++)
    {
      myFile.print(SENSOR[i]);
      myFile.print(" (");
      myFile.print(UNITS[i]);
      myFile.print(") ");
      myFile.print(", ");
    }
    myFile.print(SENSOR[11]);
    myFile.println();
    // close the file:
    myFile.close();
    #if debuggEnabled
        Serial.println(F("Closing...")); 
    #endif 
  }
}


