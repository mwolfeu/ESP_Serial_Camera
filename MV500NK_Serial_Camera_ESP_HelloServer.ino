#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include "camESPWebServer.h"
#include <ESP8266mDNS.h>

const char* ssid = "shortbus";
const char* password = "huskyraven640";

camESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial1.begin(115200);  // GPIO 02 Tx only / Rx used for onboard flash?
  
  WiFi.begin(ssid, password);
  Serial1.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  Serial1.println("");
  Serial1.print("Connected to ");
  Serial1.println(ssid);
  Serial1.print("IP address: ");
  Serial1.println(WiFi.localIP());
  Serial1.print("Data RAM: ");
  Serial1.println(ESP.getFreeHeap());

  if (MDNS.begin("esp8266")) {
    Serial1.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/cam.jpg", []() {
    server.sendCam(200);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial1.println("HTTP server started");
  server.camVerify();
}

void loop(void) {
  server.handleClient();
}
