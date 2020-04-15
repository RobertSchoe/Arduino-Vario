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
#include <Arduino.h>
#include <Scheduler.h>
#include "Vario.h"
#include "PositionLogger.h"
#include "UploadServer.h"

void setup() { 
  Serial.begin(9600); 
  Serial.println("Initializing SD card, barometer and gps sensor...");

  Scheduler.start(&vario_task);
  Scheduler.start(&gpstracking_task);
  Scheduler.start(&server_task);

  Scheduler.begin();
}

void loop(){}
