#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include "Motor.h"

#include "WifiCredentials.h"

#undef WIFI_MODE_AP

ESP8266WebServer srv(80);
WebSocketsServer webSocket(81);
Motor
  motorL(D7, D6, D5, D0),
  motorR(D1, D2, D3, D4);

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

// handle a websocket traffic. Note, that only one client connection is allowed at any time.
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  static uint8_t connectedClient = 0xff;

  switch(type) {
    case WStype_DISCONNECTED: //if a connected client disconnects, free the slot
      Serial.printf("[%u] Disconnected!\n", num);
      if (num == connectedClient) {
        connectedClient = 0xff; //'magic' number, don't want to keep another flag
      }
      break;
    case WStype_CONNECTED: { //a new client has connected
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        if (connectedClient != 0xff || num == 0xff) {
          //we must drop a connection with number 0xff, otherwise there would be a glitch
          //client should retry connecting
          webSocket.sendTXT(num, "NO WAY!");
          webSocket.disconnect(num);
        } else {
          connectedClient = num;
          webSocket.sendTXT(num, "GO ON");
        }
      }
      break;
    case WStype_TEXT: {
        if (num != connectedClient) {
          webSocket.sendTXT(num, "NO WAY!");
          webSocket.disconnect(num);
          break;
        }
//        Serial.printf("[%u] got Text: %s\n", num, payload);
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
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();

#ifdef WIFI_MODE_AP
  //set up an access point
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  if (result == true) {
    Serial.println("Ready");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  } else {
    Serial.println("Failed!");
    delay(10000);
    //don't know if it's a right thing to do...
    ESP.reset();
  }
#else
  //conect to an existing WiFi network
  Serial.print("Connecting to the WiFi.");
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
  while (WiFi.status() != WL_CONNECTED)
  //blink a LED slowly when waiting for a connection
  {
    digitalWrite(BUILTIN_LED, 1 - digitalRead(BUILTIN_LED));
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());
  //blink fast a few times to indicate a successful connection
  for(char i = 0; i < 8; i++) {
    digitalWrite(BUILTIN_LED, 1);
    delay(200);
    digitalWrite(BUILTIN_LED, 0);
    delay(200);
  }
  //TODO: need to figure out a way to show an IP address without a serial connection
#endif

  //HTTP redirect on / location -> index.html
  srv.on("/", [](){
    srv.sendHeader("Location", String("/index.html"), true);
    srv.send(302, "text/plain", "");
  });
  //default routing -> look for a file on the SPIFFS and serve
  srv.onNotFound([](){
    if(!handleFileRead(srv.uri()))
      srv.send(404, "text/plain", "FileNotFound");
  });
  //start a HTTP server on port 80
  srv.begin();
  Serial.println("HTTP server started");

  //set up a file subsystem
  if (!(SPIFFS.begin())) {
    Serial.println("SPIFFS.begin failed");
  }

  //start a websocket server on port 81
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  srv.handleClient();
  webSocket.loop();
  motorL.loop();
  motorR.loop();
}
