// HTTP Requests
// http://esp8266sd.local/index.htm?download=true (download)
// http://esp8266sd.local/index.htm               (call)
// http://esp8266sd.local/edit                    (delete)
// -----------------------------18467633426500
// Content-Disposition: form-data; name="path"
//
// /get_started_with_gopro.url
// -----------------------------18467633426500--
//
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <SD.h>

#include <ESP8266mDNS.h>

#define CS 15 //ESP PIN D8

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "12345678"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

static bool hasSD = false;
File uploadFile;

class UploadServer : public Task {
  public:
    void setup() {
      WiFi.disconnect();
      Serial.begin(9600);
      Serial.setDebugOutput(true);
      Serial.print("\n");

      WiFi.softAP(ssid, password);
      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(myIP);

      server.on("/list", HTTP_GET, std::bind(&UploadServer::printDirectory, this));
      server.on("/edit", HTTP_DELETE, std::bind(&UploadServer::handleDelete, this));
      //server.on("/edit", HTTP_PUT, std::bind(&UploadServer::handleCreate, this));
      //server.on("/edit", HTTP_POST, []() { returnOK(); }, std::bind(&UploadServer::handleFileUpload, this);
      server.onNotFound(std::bind(&UploadServer::handleNotFound, this)/*handleNotFound*/);

      server.begin();
      Serial.println("HTTP server started");

      if (SD.begin(CS)) {
        Serial.println("SD Card initialized.");
        hasSD = true;
      }
      else{
        Serial.println("SD Card NOT initialized.");
      }
  }

  void loop() {
    server.handleClient();
  }
  
  void returnOK() {
      server.send(200, "text/plain", "");
  }

    void returnFail(String msg) {
      server.send(500, "text/plain", msg + "\r\n");
    }

    bool loadFromSdCard(String path) {
      String dataType = "text/plain";
      if (path.endsWith("/")) {
        path += "index.htm";
      }

      if (path.endsWith(".src")) {
        path = path.substring(0, path.lastIndexOf("."));
      } else if (path.endsWith(".htm")) {
        dataType = "text/html";
      } else if (path.endsWith(".css")) {
        dataType = "text/css";
      } else if (path.endsWith(".js")) {
        dataType = "application/javascript";
      } else if (path.endsWith(".png")) {
        dataType = "image/png";
      } else if (path.endsWith(".gif")) {
        dataType = "image/gif";
      } else if (path.endsWith(".jpg")) {
        dataType = "image/jpeg";
      } else if (path.endsWith(".ico")) {
        dataType = "image/x-icon";
      } else if (path.endsWith(".xml")) {
        dataType = "text/xml";
      } else if (path.endsWith(".pdf")) {
        dataType = "application/pdf";
      } else if (path.endsWith(".zip")) {
        dataType = "application/zip";
      }

      File dataFile = SD.open(path.c_str());
      if (dataFile.isDirectory()) {
        path += "/index.htm";
        dataType = "text/html";
        dataFile = SD.open(path.c_str());
      }

      if (!dataFile) {
        return false;
      }

      if (server.hasArg("download")) {
        dataType = "application/octet-stream";
      }

      if (server.streamFile(dataFile, dataType) != dataFile.size()) {
        Serial.println("Sent less data than expected!");
      }

      dataFile.close();
      return true;
    }

    void deleteRecursive(String path) {
      File file = SD.open((char *)path.c_str());
      if (!file.isDirectory()) {
        file.close();
        SD.remove((char *)path.c_str());
        return;
      }

      file.rewindDirectory();
      while (true) {
        File entry = file.openNextFile();
        if (!entry) {
          break;
        }
        String entryPath = path + "/" + entry.name();
        if (entry.isDirectory()) {
          entry.close();
          deleteRecursive(entryPath);
        } else {
          entry.close();
          SD.remove((char *)entryPath.c_str());
        }
        yield();
      }

      SD.rmdir((char *)path.c_str());
      file.close();
    }
    void handleDelete() {
      if (server.args() == 0) {
        return returnFail("BAD ARGS");
      }
      String path = server.arg(0);
      if (path == "/" || !SD.exists((char *)path.c_str())) {
        returnFail("BAD PATH");
        return;
      }
      deleteRecursive(path);
      returnOK();
    }

    void printDirectory() {
      if (!server.hasArg("dir")) {
        return returnFail("BAD ARGS");
      }
      String path = server.arg("dir");
      if (path != "/" && !SD.exists((char *)path.c_str())) {
        return returnFail("BAD PATH");
      }
      File dir = SD.open((char *)path.c_str());
      path = String();
      if (!dir.isDirectory()) {
        dir.close();
        return returnFail("NOT DIR");
      }
      dir.rewindDirectory();
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "text/json", "");
      WiFiClient client = server.client();

      server.sendContent("[");
      for (int cnt = 0; true; ++cnt) {
        File entry = dir.openNextFile();
        if (!entry) {
          break;
        }

        String output;
        if (cnt > 0) {
          output = ',';
        }

        output += "{\"type\":\"";
        output += (entry.isDirectory()) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += entry.name();
        output += "\"";
        output += "}";
        server.sendContent(output);
        entry.close();
      }
      server.sendContent("]");
      server.sendContent(""); // Terminate the HTTP chunked transmission with a 0-length chunk
      dir.close();
    }

    void handleNotFound() {
      if (hasSD && loadFromSdCard(server.uri())) {
        return;
      }
      String message = "SDCARD Not Detected\n\n";
      message += "URI: ";
      message += server.uri();
      message += "\nMethod: ";
      message += (server.method() == HTTP_GET) ? "GET" : "POST";
      message += "\nArguments: ";
      message += server.args();
      message += "\n";
      for (uint8_t i = 0; i < server.args(); i++) {
        message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
      }
      server.send(404, "text/plain", message);
      Serial.print(message);
    }
    void handleFileUpload() { //not needed so far
      if (server.uri() != "/edit") {
        return;
      }
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        if (SD.exists((char *)upload.filename.c_str())) {
          SD.remove((char *)upload.filename.c_str());
        }
        uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
        Serial.print("Upload: START, filename: "); Serial.println(upload.filename);
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadFile) {
          uploadFile.write(upload.buf, upload.currentSize);
        }
        Serial.print("Upload: WRITE, Bytes: "); Serial.println(upload.currentSize);
      } else if (upload.status == UPLOAD_FILE_END) {
        if (uploadFile) {
          uploadFile.close();
        }
        Serial.print("Upload: END, Size: "); Serial.println(upload.totalSize);
      }
    }
    
    void handleCreate() {  //not needed so far
      if (server.args() == 0) {
        return returnFail("BAD ARGS");
      }
      String path = server.arg(0);
      if (path == "/" || SD.exists((char *)path.c_str())) {
        returnFail("BAD PATH");
        return;
      }

      if (path.indexOf('.') > 0) {
        File file = SD.open((char *)path.c_str(), FILE_WRITE);
        if (file) {
          file.write((const char *)0);
          file.close();
        }
      } else {
        SD.mkdir((char *)path.c_str());
      }
      returnOK();
    } 
} server_task;
