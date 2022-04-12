/*
 *** English version under construction *** 
 *  For more information feel free to contact me
 
 *  Timski projekt: e-pulse
 *  Slanje pulsa i zasićenosti krvi kisikom (Online_HR_ SpO2)
    Implementacija Thinger.io platforme
  
                  Spajanje:
            MAX30102 ->  NodeMCU
            SDA      ->  D2(GPIO4)
            SCL      ->  D1(GPIO5)

  Kod koristi SparkFun algoritam za čitanje pulsa i zasićenosti krvi kisikom
*/

#include <Wire.h>                         // Standardna Arduino I2C biblioteka
#include "MAX30105.h"                     // SparkFun MAX3010x biblioteka - za MAX30105 i MAX30102 osjetnike
#include "spo2_algorithm.h"               // Algoritam za izračunavanje zasićenosti krvi kisikom
#include <ThingerESP8266.h>               // Thinger.io biblioteka za povezivanje ESP8266 (NodeMCU) s Thinger.io platformom

#define USERNAME "mamusas"                // Korisničko ime na Thinger.io platformi
#define DEVICE_ID "ePulseESP8266_2"       // Naziv uređaja definiran na platformi
#define DEVICE_CREDENTIAL "eMbTMmM@on&k"  // Sigurnosni string za uvezivanje uređaja na platformu
#define SSID "Huawei_P20_lite"            // Naziv Wi-Fi pristupne točke
#define SSID_PASSWORD "epulse2020"        // Password Wi-Fi pristupne točke

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); // Inicijalizacija uređaja za Thinger.io platformu
MAX30105 max30102;                                            // Inicijalzacija osjetnika

uint32_t irBuffer[100];       // Bilježi vrijednost infracrvenog svitla
uint32_t redBuffer[100];      // Bilježi vrijednost crvenogsvitla
int32_t bufferLength;         // Veličina međuspremnika - ref: MAX30102 datasheet
int32_t spo2;                 // Vrijednost SpO2
int8_t validSPO2;             // Provjera je li očitanje SpO2 validno
int32_t heartRate;            // Vrijednost pulsa
int8_t validHeartRate;        // Provjera je li očitanje pulsa validno
byte pulseLED = 15;           // Indikator pulsa (PWM pin) - D8
byte readLED = 13;            // Indikator novog podataka u međuspremniku - D7

void setup()
{
  
  Serial.begin(115200);        // Otvaranje serijske komunikacije na baud rate 115200 - potrebno za debugging i ispis prvih 100 uzoraka        
    
  // Postavke ESP8266
  thing.add_wifi(SSID, SSID_PASSWORD);  
  
  // Dodavanje resursa spo2 i proslijeđivanje varijable spo2
  thing["spo2"] >> [](pson & out) {     
    out = spo2;
  };
  
  // Dodavanje resursa puls i proslijeđivanje varijable heartRate
  thing["puls"] >> [](pson & out) { 
    out = heartRate;
  };

  // Postavke ledica
  pinMode(pulseLED, OUTPUT);          
  pinMode(readLED, OUTPUT);

  // Inicijalizacija osjetnika
  if (!max30102.begin(Wire, I2C_SPEED_FAST)) //Koristi I2C, 400kHz
  {
    Serial.println(F("Osjetnik nije pronađen"));
    while (1);
  }

  byte ledBrightness = 60;  // Options: 0=Off to 255=50mA
  byte sampleAverage = 4;   // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;         // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100;    // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;     // Options: 69, 118, 215, 411
  int adcRange = 4096;      // Options: 2048, 4096, 8192, 16384

  // Postavke MAX30102
  max30102.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
}

void loop()
{
  // Međuspremnik u koji se sprema 25 uzoraka u sekundi (25Hz) - trajanje 4 sekunde
  bufferLength = 100; 

  // Pročitaj prvih 100 uzoraka te odredi prosjek signala
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (max30102.available() == false)  // Čeka nove podatke
      max30102.check();                    // Osjetnik traži nove podatke

    redBuffer[i] = max30102.getRed();
    irBuffer[i] = max30102.getIR();
    max30102.nextSample();

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }

  //Izračunaj prvih 100 uzoraka Pulsa i Spo2 (4 sekunde)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  //Uzastopno prima podatke s osjetnika. Puls i SpO2 se računaju u intervalu 1 sekunde
  while (1)
  {
    // Poziv handle() funkcije za Thinger.io platformu
    thing.handle();
    // Ako je očitanje kisika validno podatak se stream (inače prikazuje zadnje čitanje)
    
    if (validSPO2){
      thing.stream(thing["spo2"]);
    }
    
    // Ako je očitanje pulsa validno podatak se streama (inače prikazuje zadnje čitanje)
    if (validHeartRate) {
      thing.stream(thing["puls"]);
    }
    
    // Prebaci prvih 25 uzoraka u memoriju te ostalih 75 pomakni (bubble sort)
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    } 

//Uzmi 25 uzoraka prije računanja pulsa
    for (byte i = 75; i < 100; i++)
    {
      while (max30102.available() == false)  //Čeka nove podatke
        max30102.check();                    //Senzor pregledava za nove podatke
        
      // LED signal za svaki novi uzorak
      digitalWrite(readLED, !digitalRead(readLED)); 

      redBuffer[i] = max30102.getRed();
      irBuffer[i] = max30102.getIR();
      
      //Idući uzorak
      max30102.nextSample(); 
    }

    //Nakon skupljanja novih 25 uzoraka preračunaj puls i SpO2
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
}
