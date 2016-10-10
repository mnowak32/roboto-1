#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include "Motor.h"

ESP8266WebServer srv(80);
WebSocketsServer webSocket(81);
Motor motorL(D0, D5, D6, D7), motorR(D1, D2, D3, D4);

String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  String contentType = getContentType(path);
  if(SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = srv.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch(type) {
    case WStype_DISCONNECTED:
//      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      
        // send message to client
        webSocket.sendTXT(num, "GO ON");
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[%u] got Text: %s\n", num, payload);
        String cmd((char *)payload);
        char mot = cmd.charAt(0);
        int spd = cmd.substring(1).toInt();
        char dir = 'F'; //forward
        if (spd < 0) {
          dir = 'R'; //reverse
          spd = -spd;
        } else if (spd == 0) {
          dir = 'S'; //stop
        }
  
        switch(mot) {
          case 'L': motorL.setDirSpeed(dir, (uint16_t) spd); break;
          case 'R': motorR.setDirSpeed(dir, (uint16_t) spd); break;
          default: break;
        }
      }
      break;
//      case WStype_BIN:
//        Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
//        hexdump(payload, lenght);

        // send message to client
        // webSocket.sendBIN(num, payload, lenght);
//        break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("ROBOTO-1", "iknowkungfu");
  if (result == true) {
    Serial.println("Ready");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    srv.on("/", [](){
      srv.sendHeader("Location", String("/index.html"), true);
      srv.send(302, "text/plain", "");
    });
    srv.onNotFound([](){
      if(!handleFileRead(srv.uri()))
        srv.send(404, "text/plain", "FileNotFound");
    });    
    srv.begin();
    Serial.println("HTTP server started");
  } else {
    Serial.println("Failed!");
  }
  if (!(SPIFFS.begin())) {
    Serial.println("SPIFFS.begin failed");
  }

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);  
}

void loop() {
  srv.handleClient();
  webSocket.loop();
  motorL.loop();
  motorR.loop();
}
