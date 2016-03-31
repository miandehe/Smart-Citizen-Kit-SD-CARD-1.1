void sckBegin() {
  driver.begin();
  
  driver.writeCharge(350);
  
  sckWriteVH(CO_SENSOR, 2700); //VH_CO SENSOR Inicial
  digitalWrite(IO0, HIGH); //VH_CO SENSOR

  sckWriteVH(NO2_SENSOR, 1700); //VH_CO SENSOR Inicial
  digitalWrite(IO1, HIGH); //VH_NO2 SENSOR
  digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA  
  
  pinMode(IO3, OUTPUT);
  digitalWrite(IO3, HIGH); //Alimentacion de los MICS
  
  driver.writeResistor(CO_SENSOR, 100000); //Inicializacion de la carga del CO SENSOR
  driver.writeResistor(NO2_SENSOR, 100000); //Inicializacion de la carga del NO2 SENSOR
}

void sckConfig(){
  
  if (!sckCompareDate(__TIME__, driver.readData(EE_ADDR_TIME_VERSION, 0, INTERNAL)))
  {
    driver.RTCadjust(driver.sckDate(__DATE__,__TIME__));
#if debuggEnabled
    Serial.println(F("Resetting..."));
#endif
  }

}

boolean sckCompareDate(char* text, char* text1)
{
  if ((strlen(text))!=(strlen(text1))) return false;
  else 
  {
    for(int i=0; i<strlen(text); i++)
    {
      if (text[i]!=text1[i]) return false;
    }
  }
  return true;
}

uint16_t sckGetBattery() {
  uint16_t temp = driver.levelRead();
  #if !DataRaw 
    temp = map(temp, VAL_MIN_BATTERY, VAL_MAX_BATTERY, 0, 100);
    if (temp>1000) temp=1000;
    if (temp<0) temp=0;
  #endif
#if debuggSCK
  Serial.print("Vbat: ");
  Serial.print(temp);
  Serial.println(" mV, ");
#endif
  return temp; 
}

uint16_t sckGetCharger() {
  uint16_t temp = driver.panelRead();
#if debuggSCK
  Serial.print("Charger voltage: ");
  Serial.print(temp);
  Serial.print(" mV, ");
#endif
  return temp; 
}




