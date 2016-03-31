/*

  SCKDrivers.h
  Supports core and data management functions (Power, WiFi, SD storage, RTClock and EEPROM storage)

  - Modules supported:

    - WIFI
    - RTC 
    - EEPROM
    - POWER MANAGEMENT IC's

*/

#include "Constants.h"
#include "SCKDrivers.h"
#include <Wire.h>
#include <EEPROM.h>

#define debug false

void SCKDriver::begin() {
  Wire.begin();
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;  
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(IO0, OUTPUT); //VH_CO SENSOR
  pinMode(IO1, OUTPUT); //VH_NO2 SENSOR
  pinMode(IO2, OUTPUT); //NO2 SENSOR_HIGH_IMPEDANCE
  pinMode(AWAKE, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CONTROL, INPUT);
  digitalWrite(AWAKE, HIGH); 
  digitalWrite(IO0, HIGH); 
  digitalWrite(IO1, HIGH); 
}

/*RTC commands*/

#define buffer_length        32
static char buffer[buffer_length];

char* SCKDriver::sckDate(const char* date, const char* time){
    int j = 0;
    for  (int i = 7; date[i]!=0x00; i++)
      {
        buffer[j] = date[i];
        j++;
      }
    buffer[j] = '-';
    j++;
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': 
            if (date[1] == 'a') buffer[j] = '1';
            else if (date[2] == 'n') buffer[j] = '6';
            else buffer[j] = '7';
            break; 
        case 'F': 
            buffer[j] = '2'; 
            break;
        case 'A': 
            if (date[1] == 'p') buffer[j] = '4';
            else buffer[j] = '8';
            break;
        case 'M': 
            if (date[2] == 'r') buffer[j] = '3';
            else buffer[j] = '5';
            break;
        case 'S': 
            buffer[j] = '9'; 
            break;
        case 'O': 
            buffer[j] = '1'; 
            buffer[j+1] = '0';
            j++;
            break;
        case 'N': 
            buffer[j] = '1'; 
            buffer[j+1] = '1';
            j++;
            break;
        case 'D': 
            buffer[j] = '1'; 
            buffer[j+1] = '2';
            j++;
            break;
    }
  j++;
  buffer[j] = '-';
  j++;
  byte k = 4;
  if (date[4] == ' ')  k = 5;
  for  (int i = k; date[i]!=' '; i++)
      {
        buffer[j] = date[i];
        j++;
      }
  buffer[j] = ' ';
  j++;
  for  (int i = 0; time[i]!=0x00; i++)
    {
      buffer[j] = time[i];
      j++;
    }
  buffer[j]=0x00;   
  return buffer;
}

boolean SCKDriver::RTCadjust(char *time) {    
  byte rtc[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  byte count = 0x00;
  byte data_count=0;
  while (time[count]!=0x00)
  {
    if(time[count] == '-') data_count++;
    else if(time[count] == ' ') data_count++;
    else if(time[count] == ':') data_count++;
    else if ((time[count] >= '0')&&(time[count] <= '9'))
    { 
      rtc[data_count] =(rtc[data_count]<<4)|(0x0F&time[count]);
    }  
    else break;
    count++;
  }  
  if (data_count == 5)
  {
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write((int)0);
    Wire.write(rtc[5]);
    Wire.write(rtc[4]);
    Wire.write(rtc[3]);
    Wire.write(0x00);
    Wire.write(rtc[2]);
    Wire.write(rtc[1]);
    Wire.write(rtc[0]);
    Wire.endTransmission();
    delay(4);
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x0E); //Address
    Wire.write(0x00); //Value
    Wire.endTransmission();
    return true;
  }
  return false;  
}

char timeRTC[20] = {'2', '0', '0', '0', '-', '0', '0', '-', '0', '0', ' ', '0', '0', ':', '0', '0', ':', '0', '0', 0x00};

boolean SCKDriver::RTCtime(char *time) {
  uint8_t date[6] = {0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x7F};
  for (int i=0; i<20; i++) time[i] = timeRTC[i];
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((int)0);  
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 7);
  for (int i=5; i>2; i--) date[i] = (Wire.read() & date[i]);
  Wire.read();
  for (int i=2; i>=0; i--) date[i] = (Wire.read() & date[i]);
  for (int i=0; i<6; i++) 
    {
       time[2+3*i] =  (date[i]>>4) + '0';
       time[3+3*i] =  (date[i]&0x0F) + '0';
    }
  return true;
}

char* SCKDriver::RTCtime() {
  uint8_t date[6] = {0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x7F};
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((int)0);  
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 7);
  for (int i=5; i>2; i--) date[i] = (Wire.read() & date[i]);
  Wire.read();
  for (int i=2; i>=0; i--) date[i] = (Wire.read() & date[i]);
  for (int i=0; i<6; i++) 
    {
       timeRTC[2+3*i] =  (date[i]>>4) + '0';
       timeRTC[3+3*i] =  (date[i]&0x0F) + '0';
    }
  return timeRTC;
}

/*Inputs an outputs control*/

float SCKDriver::average(int anaPin) {
  int lecturas = 100;
  long total = 0;
  float average = 0;
  for(int i=0; i<lecturas; i++)
  {
    //delay(1);
    total = total + analogRead(anaPin);
  }
  average = (float)total / lecturas;  
  return(average);
}


float SCKDriver::getVcc()
{
  float temp = average(S3);
  analogReference(INTERNAL);
  delay(100);
  float Vcc = (float)(average(S3)/temp)*2560.;
  analogReference(DEFAULT);
  delay(100);
  return Vcc;
}
  
int SCKDriver::levelRead() {
    delay(10);
    return 1.553*(getVcc()/1023.)*average(LEVEL);
}

int SCKDriver::panelRead() {
    delay(10);
    uint16_t value = 11*(getVcc()/1023.)*average(PANEL); //Voltage protection diode
    if (value > 500) value = value + 120; //Voltage protection diode
    else value = 0;
    return value;
}

/*EEPROM commands*/

void SCKDriver::writeEEPROM(uint16_t eeaddress, uint8_t data) {
  uint8_t retry = 0;
  while ((readEEPROM(eeaddress)!=data)&&(retry<10))
  {  
    Wire.beginTransmission(E2PROM);
    Wire.write((byte)(eeaddress >> 8));   // MSB
    Wire.write((byte)(eeaddress & 0xFF)); // LSB
    Wire.write(data);
    Wire.endTransmission();
    delay(6);
    retry++;
  }
}

byte SCKDriver::readEEPROM(uint16_t eeaddress) {
  byte rdata = 0xFF;
  Wire.beginTransmission(E2PROM);
  Wire.write((byte)(eeaddress >> 8));   // MSB
  Wire.write((byte)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(E2PROM,1);
  while (!Wire.available()); 
  rdata = Wire.read();
  return rdata;
}

void SCKDriver::writeData(uint32_t eeaddress, long data, uint8_t location)
{
    for (int i =0; i<4; i++) 
      {
        if (location == EXTERNAL) writeEEPROM(eeaddress + (3 -i) , data>>(i*8));
        else EEPROM.write(eeaddress + (3 -i), data>>(i*8));
      }

}

void SCKDriver::writeData(uint32_t eeaddress, uint16_t pos, char* text, uint8_t location)
{
  uint16_t eeaddressfree = eeaddress + buffer_length * pos;
  if (location == EXTERNAL)
    {
      for (uint16_t i = eeaddressfree; i< (eeaddressfree + buffer_length); i++) writeEEPROM(i, 0x00);
      for (uint16_t i = eeaddressfree; text[i - eeaddressfree]!= 0x00; i++) writeEEPROM(i, text[i - eeaddressfree]);
    }
  else
    {
      
      for (uint16_t i = eeaddressfree; i< (eeaddressfree + buffer_length); i++) EEPROM.write(i, 0x00);
      for (uint16_t i = eeaddressfree; text[i - eeaddressfree]!= 0x00; i++) 
        {
          //if (eeaddressfree>=DEFAULT_ADDR_SSID) if (text[i - eeaddressfree]==' ') text[i - eeaddressfree]='$';
          if (text[i - eeaddressfree]==' ') text[i - eeaddressfree]='$';
          EEPROM.write(i, text[i - eeaddressfree]); 
        }
    }
}

uint32_t SCKDriver::readData(uint16_t eeaddress, uint8_t location)
{
  uint32_t data = 0;
  for (int i =0; i<4; i++)
    {
      if (location == EXTERNAL)  data = data + (uint32_t)((uint32_t)readEEPROM(eeaddress + i)<<((3-i)*8));
      else data = data + (uint32_t)((uint32_t)EEPROM.read(eeaddress + i)<<((3-i)*8));
    }
  return data;
}

char* SCKDriver::readData(uint16_t eeaddress, uint16_t pos, uint8_t location)
{
  eeaddress = eeaddress + buffer_length * pos;
  uint16_t i;
  if (location == EXTERNAL)
    {
      uint8_t temp = readEEPROM(eeaddress);
      for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
      {
        buffer[i - eeaddress] = readEEPROM(i);
        temp = readEEPROM(i + 1);
      }
    }
  else
    {
      uint8_t temp = EEPROM.read(eeaddress);
      for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
      {
        buffer[i - eeaddress] = EEPROM.read(i);
        temp = EEPROM.read(i + 1);
      }
    }
  buffer[i - eeaddress] = 0x00; 
  return buffer;
}

/*WIFI commands*/

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

void SCKDriver::WIFIoff()
  {
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.print(F("$$$"));
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.println();
      Serial1.println();
      Serial1.println(F("sleep"));
  }


//Charger commands*/

void SCKDriver::writeCharge(int current)
  {
     if (current < 100) current = 100;
     else if (current > 500) current = 500;
     float Rp = (1000./current)-2;  //kOhm
     float resistor = Rp*10/(10-Rp); //kOhm
     writeResistor(0, resistor*1000);  
  }

float SCKDriver::readCharge()
  {
      float resistor = readResistor(0)/1000.;    
      float current = 1000./(2+((resistor * 10)/(resistor + 10)));
      return current;
  }
    
/*Potenciometer*/ 

#define RES 256   //Resolucion de los potenciometros digitales

void SCKDriver::writeResistor(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|bitRead(data, 8) ;
  Wire.write(address);
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(4);
}

void SCKDriver::writeResistor(byte resistor, float value ) {
   byte POT = POT1;
   byte ADDR = resistor;
   int data=0x0000;
   if (value>100000) value = 100000;
   data = (int)(value/kr);
   if ((resistor==2)||(resistor==3))
     {
       POT = POT2;
       ADDR = resistor - 2;
     }
   else if((resistor==4)||(resistor==5))
     {
       POT = POT2;
       ADDR = resistor + 2;
     }
   else if ((resistor==6)||(resistor==7))
     {
       POT = POT3;
       ADDR = resistor - 6;
     }
   writeResistor(POT, ADDR, data);
}

int SCKDriver::readResistor(int deviceaddress, byte address ) {
  byte rdata = 0xFF;
  int  data = 0x0000;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|B00001100;
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,2);
  unsigned long time = millis();
  while (!Wire.available()) if ((millis() - time)>500) return 0x00;
  rdata = Wire.read(); 
  data=rdata<<8;
  while (!Wire.available()); 
  rdata = Wire.read(); 
  data=data|rdata;
  return data;
}   

float SCKDriver::readResistor(byte resistor ) {
   byte POT = POT1;
   byte ADDR = resistor;
   if ((resistor==2)||(resistor==3))
     {
       POT = POT2;
       ADDR = resistor - 2;
     }
   else if((resistor==4)||(resistor==5))
     {
       POT = POT2;
       ADDR = resistor + 2;
     }
   else if ((resistor==6)||(resistor==7))
     {
       POT = POT3;
       ADDR = resistor - 6;
     }
   return readResistor(POT, ADDR)*kr;
}   

byte SCKDriver::readI2C(int deviceaddress, byte address ) {
  byte rdata = 0xFF;
  byte  data = 0x0000;
  Wire.beginTransmission(deviceaddress);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  unsigned long time = millis();
  while (!Wire.available()) if ((millis() - time)>500) return 0x00;
  data = Wire.read(); 
  return data;
}   

void SCKDriver::writeI2C(byte deviceaddress, byte address, byte data ) {
  Wire.beginTransmission(deviceaddress);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
  delay(4);
}


