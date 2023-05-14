#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <AS5600.h> //https://github.com/RobTillaart/AS5600
#include "sdTesting.h"
#include <MotorUnit.h>

#define SDMISO 13
#define SDMOSI 11
#define SDSCK 12
#define SDCS 10

#define SDA 5
#define SCL 4

//SPIClass * spi = NULL;

#include "Wire.h"

#define TCAADDR 0x70

#define TLEncoderLine 0
#define TREncoderLine 2
#define BLEncoderLine 1
#define BREncoderLine 3

#define motorPWMFreq 2000
#define motorPWMRes 10

#define tlIn1Pin 45
#define tlIn1Channel 0
#define tlIn2Pin 21
#define tlIn2Channel 1
#define tlADCPin 18

#define trIn1Pin 42
#define trIn1Channel 2
#define trIn2Pin 41
#define trIn2Channel 3
#define trADCPin 6

#define blIn1Pin 37
#define blIn1Channel 4
#define blIn2Pin 36
#define blIn2Channel 5
#define blADCPin 8

#define brIn1Pin 9
#define brIn1Channel 6
#define brIn2Pin 3
#define brIn2Channel 7
#define brADCPin 7

//This function is used to replace webprint
void fakePrint(uint8_t client, const char* format, ...){

}

MotorUnit axisTL(tlIn1Pin, tlIn2Pin, tlADCPin, TLEncoderLine, tlIn1Channel, tlIn2Channel, &fakePrint);

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
  //   case TLEncoderLine:
  //     if(encoderTL.isConnected()){
  //       Serial.println(encoderTL.getCumulativePosition()/4096.0);
  //     } 
  //     else{
  //       Serial.println("TL Encoder not connected");
  //     }
  //     break;
  //   case TREncoderLine:
  //     if(encoderTR.isConnected()){
  //       Serial.println(encoderTR.getCumulativePosition()/4096.0);
  //     } 
  //     else{
  //       Serial.println("TR Encoder not connected");
  //     }
  //     break;
  //   case BLEncoderLine:
  //     if(encoderBL.isConnected()){
  //       Serial.println(encoderBL.getRevolutions());
  //     } 
  //     else{
  //       Serial.println("BL Encoder not connected");
  //     }
  //     break;
  //   case BREncoderLine:
  //     if(encoderBR.isConnected()){
  //       Serial.println(encoderBR.getRevolutions());
  //     } 
  //     else{
  //       Serial.println("BR Encoder not connected");
  //     }
  //     break;
  }
}

void testMotor(){
  Serial.println("Testing motor");
}

void setup() {
  //Serial setup
  Serial.begin(115200);
  //These are used for the SD card
  pinMode(14, OUTPUT);
  pinMode(35, OUTPUT);

  pinMode(SDCS, OUTPUT);
  digitalWrite(SDCS,HIGH);

  //Begin the encoder I2C
  Wire.begin(SDA, SCL);

  delay(300); //Let serial begin

  

  if (!SD.begin()) {
    Serial.println("SD initialization failed!");
  }
  else{
    Serial.println("SD initialization success!");

    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Loop");

  //LED testing
  digitalWrite(14, HIGH);
  delay(300);
  digitalWrite(14, LOW);
  digitalWrite(35, HIGH);
  delay(300);
  digitalWrite(35, LOW);

  //Encoder testing
  scanPorts();

}