#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>

ESP8266WebServer srv(80);
WebSocketsServer webSocket(81);

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

void setup() {
  Serial.begin(115200);
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
}
