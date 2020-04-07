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
#define BUZZER 0

MS5611 sensor(&Wire);

unsigned long time1 = 0;
float toneFreq, toneFreqLowpass, pressure, lowpassFast, lowpassSlow ;
short ddsAcc = 0;


void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  if(sensor.connect()>0) {
    Serial.println("Error connecting...");
    delay(500);
    setup();
  }
  ddsAcc = 0;
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
    Serial.println("Pause"); 
    noTone(BUZZER);
  }
  
  while (millis() < time1);        //loop frequency timer
  time1 += 20;
}
