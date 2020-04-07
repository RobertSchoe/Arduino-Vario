/*****************************************************************************
 * Wiring for ESP12E
 * ESP12E to MS5611
 * VIN -> VCC
 * GND -> GND
 * D1  -> SCL
 * D2  -> SDA
 * 
 * BUZZER TO D3 (ESP12e D3 (PIN0 in Arduino IDE))
 * 
 * ESP to GY-GPS6MV2
 * 
 * ESP to SD (pictures on git-hub)
 * D8  -> CS
 * D7  -> MOSI
 * D6  -> MISO
 * D5  -> CLK (SCK)
 * GND -> GND
 * 3V3 -> Vcc
 * 
 * 
 * Wiring for Arduino UNO
 * UNO to MS5611
 * 5V  -> VCC
 * GND -> GND
 * A5  -> SDA
 * A4  -> SCL
 * 
 * BUZZER TO D3
 * 
 *******************************************************************************/
#include <Wire.h>
#include <MS5611.h>
#include <SPI.h>
#include <SD.h>
#define BUZZER 0
#define CS 15

MS5611 sensor(&Wire);
File myFile;

unsigned long time1 = 0;
float toneFreq, toneFreqLowpass, pressure, lowpassFast, lowpassSlow ;
short ddsAcc = 0;


void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card and barometer...");
  
  pinMode(BUZZER, OUTPUT);
  
  if(sensor.connect()>0) {
    Serial.println("Error connecting...");
    delay(500);
    setup();
  }
  
  if (!SD.begin(15)) {
    Serial.println("initialization failed!");
    while (1);                                          //deadlock and possible restart if needed
  }
  Serial.println("initialization done.");
  printTestToSD();
  readFromSD();
}


void loop()
{
  sensor.ReadProm();
  sensor.Readout();
  
  pressure = sensor.GetPres();
  //Serial.print(pressure);

  lowpassFast = lowpassFast + (pressure - lowpassFast) * 0.1;
  lowpassSlow = lowpassSlow + (pressure - lowpassSlow) * 0.05;
  
  toneFreq = (lowpassSlow - lowpassFast) * 50;
  
  toneFreqLowpass = toneFreqLowpass + (toneFreq - toneFreqLowpass) * 0.1;
  toneFreq = constrain(toneFreqLowpass, -500, 500);

  ddsAcc = ddsAcc + ((toneFreq * 100) + 2000);
  
  if (toneFreq < 0 || ddsAcc > 0) 
    tone(BUZZER, toneFreq + 510);  
  else
  { 
    noTone(BUZZER);
  }
  
  while (millis() < time1);        //loop frequency timer
  time1 += 20;
}

void printTestToSD(){
  myFile = SD.open("test.txt", FILE_WRITE);

    if (myFile) {
      Serial.print("Writing to test.txt...");
      myFile.println("testing 1, 2, 3.");
      myFile.close();
      Serial.println("done.");
    }
    else {
      Serial.println("error opening test.txt");
    }
}

void readFromSD(){
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } 
  else {
    Serial.println("error opening test.txt");
  }
}
