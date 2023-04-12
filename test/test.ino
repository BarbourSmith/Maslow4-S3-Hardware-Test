#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <AS5600.h> //https://github.com/RobTillaart/AS5600
#include "sdTesting.h"

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

void testMotor(int in1Channel, int in2Channel, int adcPin){
  Serial.println(analogRead(adcPin));
  ledcWrite(in1Channel, 500); //Half speed one way
  ledcWrite(in2Channel, 0);
  delay(500);
  Serial.println(analogRead(adcPin));
  delay(500);

  ledcWrite(in1Channel, 0); //Stop
  ledcWrite(in2Channel, 0);
  delay(300);

  ledcWrite(in1Channel, 0); //Half speed the other way
  ledcWrite(in2Channel, 500);
  delay(500);
  Serial.println(analogRead(adcPin));
  delay(500);

  ledcWrite(in1Channel, 0); //Stop
  ledcWrite(in2Channel, 0);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(35, OUTPUT);

  pinMode(SDCS, OUTPUT);
  digitalWrite(SDCS,HIGH);

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
  
  //Setup the motor controllers
  ledcSetup(tlIn1Channel, motorPWMFreq, motorPWMRes);  // configure PWM functionalities...this uses timer 0 (channel, freq, resolution)
  ledcAttachPin(tlIn1Pin, tlIn1Channel);  // attach the channel to the GPIO to be controlled
  ledcWrite(tlIn1Channel, 0); //Turn the fan off

  ledcSetup(tlIn2Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(tlIn2Pin, tlIn2Channel);
  ledcWrite(tlIn2Channel, 0);

  ledcSetup(trIn1Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(trIn1Pin, trIn1Channel);
  ledcWrite(trIn1Channel, 0);

  ledcSetup(trIn2Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(trIn2Pin, trIn2Channel);
  ledcWrite(trIn2Channel, 0);

  ledcSetup(blIn1Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(blIn1Pin, blIn1Channel);
  ledcWrite(blIn1Channel, 0);

  ledcSetup(blIn2Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(blIn2Pin, blIn2Channel);
  ledcWrite(blIn2Channel, 0);

  ledcSetup(brIn1Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(brIn1Pin, brIn1Channel);
  ledcWrite(brIn1Channel, 0);

  ledcSetup(brIn2Channel, motorPWMFreq, motorPWMRes);
  ledcAttachPin(brIn2Pin, brIn2Channel);
  ledcWrite(brIn2Channel, 0);

  //Test the SD Card connection
  //spi = new SPIClass();
  //spi->begin(SDCS, SDMISO, SDMOSI, SDCS);

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
  Serial.println(MOSI);

  //LED testing
  digitalWrite(14, HIGH);
  delay(300);
  digitalWrite(14, LOW);
  digitalWrite(35, HIGH);
  delay(300);
  digitalWrite(35, LOW);

  //Encoder testing
  scanPorts();

  readFromEncoder(TLEncoderLine);
  readFromEncoder(TREncoderLine);
  readFromEncoder(BLEncoderLine);
  readFromEncoder(BREncoderLine);

  //Motor testing
  Serial.println("Testing top left motor");
  testMotor(tlIn1Channel, tlIn2Channel, tlADCPin);
  Serial.println("Testing top right motor");
  testMotor(trIn1Channel, trIn2Channel, trADCPin);
  Serial.println("Testing bottom left motor");
  testMotor(blIn1Channel, blIn2Channel, blADCPin);
  Serial.println("Testing bottom right motor");
  testMotor(brIn1Channel, brIn2Channel, brADCPin);
}