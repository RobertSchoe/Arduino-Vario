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
