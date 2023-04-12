#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <AS5600.h> //https://github.com/RobTillaart/AS5600

#define SDMISO 13
#define SDMOSI 11
#define SDSCK 12
#define SDCS 10

#define SDA 5
#define SCL 4

SPIClass * spi = NULL;

#include "Wire.h"

#define TCAADDR 0x70

#define TLEncoderLine 0
#define TREncoderLine 2
#define BLEncoderLine 1
#define BREncoderLine 3


AS5600 encoderTL;
AS5600 encoderTR;
AS5600 encoderBL;
AS5600 encoderBR;

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

void readFromEncoder(int encoderNumber){
  tcaselect(encoderNumber);
  
  switch(encoderNumber){
    case TLEncoderLine:
      if(encoderTL.isConnected()){
        Serial.println(encoderTL.readAngle());
      } 
      else{
        Serial.println("TL Encoder not connected");
      }
      break;
    case TREncoderLine:
      if(encoderTR.isConnected()){
        Serial.println(encoderTR.readAngle());
      } 
      else{
        Serial.println("TR Encoder not connected");
      }
      break;
    case BLEncoderLine:
      if(encoderBL.isConnected()){
        Serial.println(encoderBL.readAngle());
      } 
      else{
        Serial.println("BL Encoder not connected");
      }
      break;
    case BREncoderLine:
      if(encoderBR.isConnected()){
        Serial.println(encoderBR.readAngle());
      } 
      else{
        Serial.println("BR Encoder not connected");
      }
      break;
  }


}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(35, OUTPUT);

  //Begin the encoder I2C
  Wire.begin(SDA, SCL);

  delay(300); //Let serial begin

  //Begin the encoders
  Serial.println("Connecting the encoders (TL, TR, BL, BR)");
  tcaselect(TLEncoderLine);
  encoderTL.begin();
  Serial.println(encoderTL.isConnected());
  tcaselect(TREncoderLine);
  encoderTR.begin();
  Serial.println(encoderTR.isConnected());
  tcaselect(BLEncoderLine);
  encoderBL.begin();
  Serial.println(encoderBL.isConnected());
  tcaselect(BREncoderLine);
  encoderBR.begin();
  Serial.println(encoderBR.isConnected());

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

  readFromEncoder(TLEncoderLine);
  readFromEncoder(TREncoderLine);
  readFromEncoder(BLEncoderLine);
  readFromEncoder(BREncoderLine);
}