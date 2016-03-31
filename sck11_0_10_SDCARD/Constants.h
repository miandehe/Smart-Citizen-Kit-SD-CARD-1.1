#define debuggSCK     false
#define decouplerComp true
#define DataRaw       true

#define TWI_FREQ 400000L //Frecuencia bus I2C

/* 

ARDUINO ports definitions - GPIOS and ADCs 

*/

#define AWAKE   4      // WIFLY AWAKE
#define PANEL   A8     // PANEL LEVEL
#define LEVEL   A7     // BAT LEVEL 

#define IO0 5         // CO SENSOR_HEATHER
#define IO1 13        // NO2 SENSOR_HEATHER
#define IO2 9         
#define IO3 10   
#define FACTORY 7     // WIFLY - Factory RESET/AP RN131
#define CONTROL 12    // WIFLY - CONTROL
#define CS_MMC 11     // MMC - CONTROL

#define S0 A4         //CO_SENSOR
#define S1 A5         //NO2_SENSOR
#define S2 A2         //SENS_CO CURRENT
#define S3 A3         //SENS_NO2 CURRENT
#define S4 A0         //MICRO
#define S5 A1         

#define TIME_UPDATE  30     //Tiempo entre actualizacion y actualizacion

/* 
i2c ADDRESSES 
*/

#define RTC_ADDRESS          0x68    // Address of the RTC
#define E2PROM               0x50    // Address of the EEPROM
#define ACCEL                0x53    // Address of the ADXL345

#define POT1                 0x2D    // Address of the Potenciometer 1 for battery charger  
#define POT2                 0x2E    // Address of the Potenciometer 2 for MICS 
#define POT3                 0x2F    // Address of the Potenciometer 3 for audio sensor

#define BH1730               0x29    // Direction of the light sensor
#define Temperature          0x40    // Direction of the sht25    

//Espacio reservado para los parametros de configuracion del SCK  
#define EE_ADDR_TIME_VERSION                        0   //32BYTES 


#define CO_SENSOR 0x04
#define NO2_SENSOR 0x05

#define  Rc0  10. //Ohm  Resistencia medida de corriente en el sensor CO sensor
#define  Rc1  39. //Ohm Resistencia medida de corriente en el sensor NO2 sensor
#define  R2  12000. //Ohm Resistencia de los reguladores
#define  kr  390.625     //Constante de conversion a resistencia de potenciometrosen ohmios

//#define  VCC   3300. //mV 
#define  VMICS 2734.

#define  VAL_MAX_BATTERY                             4200
#define  VAL_MIN_BATTERY                             3000

