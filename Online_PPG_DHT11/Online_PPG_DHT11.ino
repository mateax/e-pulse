    /* 
 *** English version under construction *** 
 *   For more information feel free to contact me
 
 *  Timski projekt: e-Pulse
    Streaming PPG valnog oblika, temperature i vlage (Online_PPG_DHT11.ino) 
 
    Implementacija Thinger.io platforme
    
                    Spajanje:
              MAX30102 ->  NodeMCU
              SDA      ->  D2(GPIO4)
              SCL      ->  D1(GPIO5)
  
    Kod koristi SparkFun algoritam za čitanje infracrvenog zračenja
  */
  
  #include <Wire.h>                         // Standardna Arduino I2C biblioteka
  #include "MAX30105.h"                     // SparkFun MAX3010x biblioteka - za MAX30105 i MAX30102 osjetnike
  #include "heartRate.h"                    // Algoritam za izračunavanje pulsa i snimanje PPG
  #include <ThingerESP8266.h>               // Thinger.io biblioteka za povezivanje ESP8266 (NodeMCU) s Thinger.io platformom
  #include "DHT.h"                          // Biblioteka za osjetnik DHT11 - temperatura, vlaga
  
  #define USERNAME "mamusas"                // Korisničko ime na Thinger.io platformi
  #define DEVICE_ID "ePulseESP8266"         // Naziv uređaja definiran na platformi
  #define DEVICE_CREDENTIAL "mpfs@LOp8s4v"  // Sigurnosni string za uvezivanje uređaja na platformu
  #define SSID "Huawei_P20_lite"             // Naziv Wi-Fi pristupne točke
  #define SSID_PASSWORD "epulse2020"        // Password Wi-Fi pristupne točke
  #define DHTTYPE DHT11
  
  ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); // Inicijalizacija uređaja za Thinger.io platformu
  MAX30105 max30102;                                            // Inicijalzacija osjetnika MAX30102
  
  // DHT11 pin
  const int DHTPin = 12;                                      
  DHT dht(DHTPin, DHTTYPE);                                     // Inicijalzacija osjetnika DHT11
  
  // Temperatura i vlaga zraka
  static char celsiusTemp[7];
  static char humidityTemp[7];

  
  void setup()
  {
    dht.begin(); // Metoda za početak rada DHT11
    
    // Postavke ESP8266
    thing.add_wifi(SSID, SSID_PASSWORD);
    
    // Dodavanje resursa dht11 i proslijeđivanje funkcija
    // readTemperature() u polje temp,
    // readHumitidty() u polje hum
    thing["dht11"] >> [](pson & out) {
      out["temp"] = dht.readTemperature();
      out["hum"] = dht.readHumidity();
      };

      
      // Dodavanje resursa puls i prosliđivanje funkcije
      // getIR() za očitanje infracrvenog zračenja
    thing["puls"] >> [](pson & out) {
      out = max30102.getIR();
    };
    
    // Inicijalizacija senzora
    if (!max30102.begin(Wire, I2C_SPEED_FAST))   //Koristi I2C, 400kHz
    {
      Serial.println(F("Osjetnik nije pronađen"));
      while (1);
    }
  
    //Postavke za PPG val
    byte ledBrightness = 0x1F;  //Options: 0=Off to 255=50mA
    byte sampleAverage = 8;     //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;           //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 100;       //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;       //Options: 69, 118, 215, 411
    int adcRange = 4096;        //Options: 2048, 4096, 8192, 16384
  
    // Postavke osjetnika
    max30102.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    max30102.setPulseAmplitudeRed(0x0A);
  }
  
  void loop() {
    thing.handle();               // Poziv handle() funkcije za Thinger.io platformu
    thing.stream(thing["puls"]);  // Stream pulsa
  }
  
  
