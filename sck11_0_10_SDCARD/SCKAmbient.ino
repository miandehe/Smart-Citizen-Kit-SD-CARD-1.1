
//Valores por defecto de la resistencia en vacio de los MICS
float RoCO  = 750000;
float RoNO2 = 2200;

  void sckWriteGAIN(float GAIN1, float GAIN2)
  {
    driver.writeResistor(6, GAIN1);
    delay(20);
    driver.writeResistor(7, GAIN2);
  }

  float sckReadGAIN()
  {
    return ((driver.readResistor(6)/1000)*(driver.readResistor(7)/1000));
  }    
 


  void sckWriteVH(byte device, long voltage ) {
    float resistor = ((voltage/410.)-1)*R2;
    driver.writeResistor(device - 2, resistor);
  }

  
  float sckReadVH(byte device) {
    float resistor = driver.readResistor(device - 2);
    float voltage = ((resistor/R2) + 1)*410;
    return(voltage);
  }
  
  void sckHeat(byte device, int current)
  {
    float VCC = driver.getVcc();
    float Rc=Rc0;
    byte Sensor = S2;
    if (device == NO2_SENSOR) { Rc=Rc1; Sensor = S3;}

    float Vc = (float)driver.average(Sensor)*VCC/1023; //mV 
    float current_measure = Vc/Rc; //mA 
    float Rh = (sckReadVH(device)- Vc)/current_measure;
    float Vh = (Rh + Rc)*current;

    sckWriteVH(device, Vh);
      #if debuggSCK
        if (device == NO2_SENSOR) Serial.print("NO2 SENSOR current: ");
        else Serial.print("CO SENSOR corriente: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        if (device == NO2_SENSOR) Serial.print("NO2 SENSOR correction VH: ");
        else  Serial.print("CO SENSOR correccion VH: ");
        Serial.print(sckReadVH(device));
        Serial.println(" mV");
        Vc = (float)driver.average(Sensor)*VCC/1023; //mV 
        current_measure = Vc/Rc; //mA 
        if (device == NO2_SENSOR) Serial.print("NO2 SENSOR corrected current: ");
        else Serial.print("CO SENSOR corrected current: ");
        Serial.print(current_measure);
        Serial.println(" mA");
        Serial.println("Heating...");
      #endif
    
  }

   float sckReadRs(byte device)
   {
     float VCC = driver.getVcc();
     byte Sensor = S0;
     if (device == NO2_SENSOR) {Sensor = S1; }
     float RL = driver.readResistor(device); //Ohm
     float VL = ((float)driver.average(Sensor)*VCC)/1023; //mV
     if (VL > VMICS) VL = VMICS;
     float Rs = ((VMICS-VL)/VL)*RL; //Ohm
     #if debuggSCK
        if (device == CO_SENSOR) Serial.print("CO SENSOR VL: ");
        else Serial.print("NO2 SENSOR VL: ");
        Serial.print(VL);
        Serial.print(" mV, RS: ");
        Serial.print(Rs);
        Serial.print(" Ohm, RL: ");
        Serial.print(RL);
        Serial.println(" Ohm");
     #endif;  
     return Rs;
   }
   
  float sckReadMICS(byte device)
  {
      float Rs = sckReadRs(device);
      float RL = driver.readResistor(device); //Ohm
      /*Correccion de impedancia de carga*/
      if ((Rs <= (RL - 1000))||(Rs >= (RL + 1000)))
      {
        if (Rs < 2000) driver.writeResistor(device, 2000);
        else driver.writeResistor(device, Rs);
        delay(100);
        Rs = sckReadRs(device);
      }
       return Rs;
  }
  
  void sckGetMICS(float* __RsCO, float* __RsNO2){          
       
        /*Correccion de la tension del Heather*/
        sckHeat(CO_SENSOR, 32); //Corriente en mA
        sckHeat(NO2_SENSOR, 26); //Corriente en mA
        
        *__RsCO = sckReadMICS(CO_SENSOR);
        *__RsNO2 = sckReadMICS(NO2_SENSOR);
         
  }
         
  uint16_t sckGetLight(){
      uint8_t TIME0  = 0xDA;
      uint8_t GAIN0 = 0x00;
      uint8_t DATA [8] = {0x03, TIME0, 0x00 ,0x00, 0x00, 0xFF, 0xFF ,GAIN0} ;
      
      uint16_t DATA0 = 0;
      uint16_t DATA1 = 0;
      
      Wire.beginTransmission(BH1730);
      Wire.write(0x80|0x00);
      for(int i= 0; i<8; i++) Wire.write(DATA[i]);
      Wire.endTransmission();
      delay(100); 
      Wire.beginTransmission(BH1730);
      Wire.write(0x94);	
      Wire.endTransmission();
      Wire.requestFrom(BH1730, 4);
      DATA0 = Wire.read();
      DATA0=DATA0|(Wire.read()<<8);
      DATA1 = Wire.read();
      DATA1=DATA1|(Wire.read()<<8);
        
      uint8_t Gain = 0x00; 
      if (GAIN0 == 0x00) Gain = 1;
      else if (GAIN0 == 0x01) Gain = 2;
      else if (GAIN0 == 0x02) Gain = 64;
      else if (GAIN0 == 0x03) Gain = 128;
      
      float ITIME =  (256- TIME0)*2.7;
      
      float Lx = 0;
      float cons = (Gain * 100) / ITIME;
      float comp = (float)DATA1/DATA0;

      
      if (comp<0.26) Lx = ( 1.290*DATA0 - 2.733*DATA1 ) / cons;
      else if (comp < 0.55) Lx = ( 0.795*DATA0 - 0.859*DATA1 ) / cons;
      else if (comp < 1.09) Lx = ( 0.510*DATA0 - 0.345*DATA1 ) / cons;
      else if (comp < 2.13) Lx = ( 0.276*DATA0 - 0.130*DATA1 ) / cons;
      else Lx=0;
      
       #if debuggSCK
        Serial.print("BH1730: ");
        Serial.print(Lx);
        Serial.println(" Lx");
      #endif
     return Lx;
  }
 
  
  unsigned int sckGetNoise() {  
    float VCC = driver.getVcc();
    #define GAIN1 100000
    #define GAIN2 100000
    sckWriteGAIN(GAIN1,GAIN2);  
    float mVRaw = (float)((driver.average(S4))/1023.)*VCC;
    float dB = 0;
    
    #if debuggSCK
      Serial.print("nOISE: ");
      Serial.print(mVRaw);
      Serial.print(" mV, P6: ");
      Serial.print(driver.readResistor(6));
      Serial.print(", P7: ");
      Serial.print(driver.readResistor(7));
      Serial.print(", GAIN: ");
      Serial.println(sckReadGAIN());
    #endif
    return mVRaw; 
  }


/*Sensor temperature*/
  
uint16_t sckReadSHT(uint8_t type){
      uint16_t DATA = 0;
      Wire.beginTransmission(Temperature);
      Wire.write(type);
      Wire.endTransmission();
      Wire.requestFrom(Temperature,2);
      unsigned long time = millis();
      while (!Wire.available()) if ((millis() - time)>500) return 0x00;
      DATA = Wire.read()<<8; 
      while (!Wire.available()); 
      DATA = (DATA|Wire.read()); 
      DATA &= ~0x0003; 
      return DATA;
  }
  
void sckGetSHT(float* __Temperature, float* __Humidity)
   {
        *__Temperature = (-46.85 + (175.72*(sckReadSHT(0xE3)/65536.0)));
        *__Humidity    = (-6 + (125*(sckReadSHT(0xE5)/65536.0))); 
        
    }

/*Accelerometer*/    

void sckAccelDefault(void)
{
    driver.writeI2C(ACCEL, 0x2D, 0x08);
//    driver.writeI2C(ACCEL, 0x31, 0x00); //2g
//    driver.writeI2C(ACCEL, 0x31, 0x01); //4g
    driver.writeI2C(ACCEL, 0x31, 0x02); //8g
//    driver.writeI2C(ACCEL, 0x31, 0x03); //16g
}

byte acc[6];

void sckReadAcc(float* __x, float* __y, float* __z)
  {
    Wire.beginTransmission(ACCEL);
    Wire.write(0x32);
    Wire.endTransmission();
    Wire.requestFrom(ACCEL, 6);
    for (int i=0; i<6; i++) acc[i] = Wire.read();
    *__x = (uint32_t)((int32_t)(acc[0] << 8 | acc[1]) + 32768);
    *__y = (uint32_t)((int32_t)(acc[2] << 8 | acc[3]) + 32768);
    *__z = (uint32_t)((int32_t)(acc[4] << 8 | acc[5]) + 32768);
  }






