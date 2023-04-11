#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define SDMISO 13
#define SDMOSI 11
#define SDSCK 12
#define SDCS 10

#define SDA 5
#define SCL 4

SPIClass * spi = NULL;

#include "Wire.h"

#define TCAADDR 0x70

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void scanPorts(){
  Serial.println("\nTCAScanner ready!");
    
  for (uint8_t t=0; t<4; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;

      Wire.beginTransmission(addr);
      if (!Wire.endTransmission()) {
        Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
      }
    }
  }
  Serial.println("\ndone");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(35, OUTPUT);

  //Test the encoder connection
  Wire.begin(SDA, SCL);

  //Test the SD Card connection
  spi = new SPIClass();
  spi->begin(SDCS, SDMISO, SDMOSI, SDCS);

  if (!SD.begin(SDCS, *spi)) {
    Serial.println("SD initialization failed!");
  }
  else{
    Serial.println("SD initialization success!");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Loop");
  delay(300);
  digitalWrite(14, HIGH);
  delay(300);
  digitalWrite(14, LOW);
  digitalWrite(35, HIGH);
  delay(300);
  digitalWrite(35, LOW);

  scanPorts();
}