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
char path[23];
bool headerwritten;

class GPSTask : public Task{
  
  protected:
  void setup(){
    Serial.println("setup sd task");
    setupSD();
    getPath();
  }
  
  void loop(){
    
    if(lat != 0.0 && lon!= 0.0){
      headerwritten = headerwritten == true ? true : writeFileHeader();
      writeGPSDataToSD();
    }
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
    myFile = SD.open(path, FILE_WRITE);

    if (myFile) {
      Serial.print("Writing to path.txt...");

      myFile.print(thour);    myFile.print(":");
      myFile.print(tminute);  myFile.print(":");
      myFile.print(tsecond);  myFile.print("Z-");
      myFile.print(lon, 5);   myFile.print(" ");
      myFile.print(lat, 5);   myFile.print(" ");
      myFile.println(alt,0);

      myFile.close();
      Serial.println("done.");
    }
    else {
      Serial.println("error opening path.txt Resetting SD...");
      setupSD();
    }
  }

  /*void readFromSD(){ // not needed so far
    myFile = SD.open(path);
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
  }*/
  void getPath(){
    String spath;
    
    for(int i=100000; i<999999; i++){
      spath = "routes/";
      spath+=i;
      spath+=".txt";
      
      Serial.println(spath);
      spath.toCharArray(path, 23);
      
      if (SD.exists(spath)) {
        Serial.print(spath);
        Serial.println(" exists.                 ");
      }
      else {
        Serial.print(path);
        Serial.println(" doesnt exist.         ");
        return;
      }
    }
  }
  bool writeFileHeader(){
    myFile = SD.open(path, FILE_WRITE);

    if (myFile) {
      myFile.print(tyear);
      myFile.print("-");
      myFile.print(tmonth);
      myFile.print("-");
      myFile.print(tday);
      myFile.println("T");

      myFile.close();
      Serial.println("File header written.");
    }
    else {
      Serial.println("Writing file header failed.");
      setupSD();
    }
  }
} gpstracking_task;
