 /*****************************************************************************
 * Wiring for ESP12E
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
 *******************************************************************************/
#include <SPI.h>
#include <SD.h>

#define CS 15 //ESP PIN D8


File myFile;

class GPSTask : public Task{
  
  protected:
  void setup(){
    Serial.println("setup sd task");
    setupSD();
  }
  
  void loop(){
    myFile = SD.open("path.txt", FILE_WRITE);
    writeGPSDataToSD();
    myFile.close();
    delay(4000);
  }

  private:
  void setupSD(){
    if (!SD.begin(CS)){                                //conntecting to SD card
      Serial.println("initialization failed!");
      //while (1);                                        //deadlock and possible restart if needed
    }
    Serial.println("initialization done.");
    //readFromSD();
  }
  
  void writeGPSDataToSD(){
    //myFile = SD.open("path.txt", FILE_WRITE);

    if (myFile) {
      Serial.print("Writing to path.txt...");
      
      myFile.print(lat, 6);
      myFile.print(",");
      myFile.print(lon, 6);
      myFile.print(",");
      myFile.println(alt,1);

      //myFile.close();
      Serial.println("done.");
    }
    else {
      Serial.println("error opening path.txt Resetting SD...");
      setupSD();
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
      setup();
    }
  }
} gpstracking_task;
