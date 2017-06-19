#include <ESP8266WebServer.h>

// for camera
#include <SoftwareSerial.h>
#include <Adafruit_VC0706.h>

class camESP8266WebServer: public ESP8266WebServer {
  protected:
    //camESP8266WebServer(int port): ESP8266WebServer(port) {};
    using ESP8266WebServer::ESP8266WebServer;
  public:
    void camVerify();
    void sendContentCam(size_t jpglen);
    void sendCam(int code);
};
