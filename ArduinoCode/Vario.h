 /*****************************************************************************
 * Wiring for ESP12E
 * ESP12E to MS5611
 * VIN -> VCC
 * GND -> GND
 * D1  -> SCL
 * D2  -> SDA
 * 
 * BUZZER TO D3 (ESP12e D3 (PIN0 in Arduino IDE))
 *******************************************************************************/
#include <TinyGPS.h>
#include <Wire.h>
#include <MS5611.h>
#include <SoftwareSerial.h>

#define BUZZER 16 //ESP PIN D0 !!!PWM needed???!!!

SoftwareSerial ss(0, 2);
MS5611 sensor(&Wire);
TinyGPS gps;


//variables for GPS
float lat, lon, alt;
unsigned long age;
bool newData;


class SoundTask : public Task{
  //variables for beeping and MS5611
  unsigned long time1 = 0;
  float toneFreq = 0, toneFreqLowpass = 0, pressure = 0, lowpassFast = 0, lowpassSlow = 0;
  short ddsAcc = 0, counter = 0;
  bool readGPS;
  
  protected:
  void setup(){
    time1 = millis() - 20;
    readGPS = 0;
    pinMode(BUZZER, OUTPUT);
    setupMS5611();
    setupGPS();
  }
  
  void loop(){
    beep();

    readGPS = readGPS == 1 ? 0 : 1;

    if(readGPS){
      getGPSData();
    }

    while (millis() < time1);        //loop frequency timer
      time1 += 20;
  }
  
  private:
  void setupGPS(){
    ss.begin(9600);         //boudrate GPS module
  }
  void setupMS5611(){
    Serial.println("setup soundtask");
    if(sensor.connect()>0) {                            //connecting to MS5611
      Serial.println("Error connecting...");
      delay(500);
      setup();
    }
  }

  void getGPSData(){             //stored in lat, lon and alt
    unsigned long lage;
    for (unsigned long start = millis(); millis() - start < 1000;)
    {
      sensor.ReadProm();                                  //calibration for MS5611
      sensor.Readout();
    
      pressure = sensor.GetPres();                        //create output sound
      beep();
      
      while (ss.available())
      {   
        char c = ss.read();
        //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
        if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;       
      }
    }
    if (newData){
      gps.f_get_position(&lat, &lon, &age);
      alt = gps.f_altitude();
      //speed = gps.f_speed_kmph();
      printGPSData();
    }
    else
      Serial.println("no new gps data");
  }
  
  void beep(){
    sensor.ReadProm();                                  //calibration for MS5611
    sensor.Readout();
  
    pressure = sensor.GetPres();    
  
    //create output sound
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
  void printGPSData(){
    Serial.print("lat=");
    Serial.print(lat, 6);
    Serial.print("   lon=");
    Serial.print(lon, 6);
    Serial.print("   alt=");
    Serial.println(alt, 1);
  }

} vario_task;
