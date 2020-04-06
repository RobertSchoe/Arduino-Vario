/*****************************************************************************
 * Wiring for ESP12e 
 * ESP12e to MS5611
 * VIN -> VCC
 * GND -> GND
 * D1  -> SCL
 * D2  -> SDA
 * 
 * Wiring for Arduino UNO
 * 5V  -> VCC
 * GND -> GND
 * A5  -> SDA
 * A4  -> SCL
 * 
 * BUZZER TO D3 (ESP12e D3 (PIN0 in Arduino IDE))
 * 
 *******************************************************************************/
#include <Wire.h>
#include <MS5611.h>
#define BUZZER 3

MS5611 sensor(&Wire);

unsigned long time1 = 0;
float toneFreq, toneFreqLowpass, pressure, lowpassFast, lowpassSlow ;
int ddsAcc;


void setup() {
  Serial.begin(9600);
  if(sensor.connect()>0) {
    Serial.println("Error connecting...");
    delay(500);
    setup();
  }
}


void loop()
{
  sensor.ReadProm();
  sensor.Readout();
  
  pressure = sensor.GetPres();
  Serial.println(pressure);
  
  lowpassFast = lowpassFast + (pressure - lowpassFast) * 0.1;
  lowpassSlow = lowpassSlow + (pressure - lowpassSlow) * 0.05;
  
  toneFreq = (lowpassSlow - lowpassFast) * 50;
  
  toneFreqLowpass = toneFreqLowpass + (toneFreq - toneFreqLowpass) * 0.1;
   
  toneFreq = constrain(toneFreqLowpass, -500, 500);
  
  ddsAcc += toneFreq * 100 + 2000;
  
  if (toneFreq < 0 || ddsAcc > 0) 
  {
    tone(BUZZER, toneFreq + 510);  
  }
  else
  {
    noTone(BUZZER);
  }
  
  while (millis() < time1);        //loop frequency timer
  time1 += 20;
}

//Funktioniert aktuell nur mit Arduino UNO und nicht mit ESP12e
