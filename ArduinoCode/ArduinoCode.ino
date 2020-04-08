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
 * 3V3 -> Vcc
 * D4  -> Rx
 * D3  -> Tx
 * GND -> GND
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
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define BUZZER 16 //ESP PIN D0 !!!PWM needed???!!!
#define CS 15 //ESP PIN D8

MS5611 sensor(&Wire);
File myFile;
TinyGPS gps;
SoftwareSerial ss(0, 2);

//variables for beeping and MS5611
unsigned long time1 = 0;
float toneFreq, toneFreqLowpass, pressure, lowpassFast, lowpassSlow ;
short ddsAcc = 0;

//variables for GPS
bool newData;
float lat, lon, alt;
unsigned long age;

void setup() {
  pinMode(BUZZER, OUTPUT);
  
  Serial.begin(9600);
  Serial.print("Initializing SD card, barometer and ...");

  setupSD();
  setupMS5611();
  setupGPS();
}


void loop()
{
  sensor.ReadProm();                                  //calibration for MS5611
  sensor.Readout();
  
  pressure = sensor.GetPres();                        //create output sound
  beep();

  //getGPSData(); //need to multithread
  //writeGPSDataToSD();
  
  while (millis() < time1);        //loop frequency timer
  time1 += 20;
}



void setupSD(){
  if (!SD.begin(15)) {                                //conntecting to SD card
    Serial.println("initialization failed!");
    while (1);                                        //deadlock and possible restart if needed
  }
  Serial.println("initialization done.");
  printTestToSD();
  readFromSD();
}


void setupMS5611(){
  if(sensor.connect()>0) {                            //connecting to MS5611
    Serial.println("Error connecting...");
    delay(500);
    setup();
  }
}


void setupGPS(){
    ss.begin(9600);         //boudrate GPS module
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

void beep(){
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
}

void getGPSData(){             //stored in lat, lon and alt
  unsigned long lage;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    /*while (ss.available())
    {
      char c = ss.read();
      //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }*/
  }

  if (newData)
  {
    gps.f_get_position(&lat, &lon, &age);
    alt = gps.f_altitude();
    //speed = gps.f_speed_kmph();
  }
}

void printGPSData(){
  Serial.print("lat=");
  Serial.print(lat, 6);
  Serial.print("   lon=");
  Serial.print(lon, 6);
  Serial.print("   alt=");
  Serial.println(alt);
}

void writeGPSDataToSD(){
    myFile = SD.open("path.txt", FILE_WRITE);

    if (myFile) {
      Serial.print("Writing to test.txt...");
      
      myFile.print(lat);
      myFile.print(",");
      myFile.print(lon);
      myFile.print(",");
      myFile.println(alt);
      
      myFile.close();
      Serial.println("done.");
    }
    else {
      Serial.println("error opening test.txt");
    }
}
