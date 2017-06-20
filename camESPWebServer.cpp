#include "camESPWebServer.h"

// https://github.com/me-no-dev/ESPAsyncWebServer#why-should-you-care
// Would have been a better choice as it deals w serial &| chunks natively.
// In chrome console you get the error net::ERR_CONTENT_LENGTH_MISMATCH a lot
// I think this has to do with softserial not keeping up

/*************************
   Camera code starts here
 *************************/

//Warning: SoftwareSerial loses bits even at low speeds
//SoftwareSerial cameraconnection(14, 12, false, 256); // RX, TX
//Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

// or use the hardware
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial);

void camESP8266WebServer::camVerify() {
  // Try to locate the camera
  if (cam.begin()) {
    Serial1.println("Camera Found:");
  } else {
    Serial1.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial1.print("Failed to get version");
  } else {
    Serial1.println("-----------------");
    Serial1.print(reply);
    Serial1.println("-----------------");
  }
}

// somehow not defined
#define min(a,b) (((a)<(b))?(a):(b))

void camESP8266WebServer::sendContentCam(size_t jpglen) {
  const char * footer = "\r\n";

  // Read all the data up to # bytes!
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buf;
    uint8_t bytesToRead = min(64, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buf = cam.readPicture(bytesToRead);

    _currentClient.write((char *)buf, bytesToRead);

    //  imgFile.write(buffer, bytesToRead);
    //  if (++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up

    Serial1.print('.');

    // Serial1.print("Read ");  Serial1.print(bytesToRead, DEC); Serial1.println(" bytes");
    jpglen -= bytesToRead;
  }

  //_currentClient.write(content.c_str(), len);

  _currentClient.write(footer, 2);
  Serial1.println("done!");
}

void camESP8266WebServer::sendCam(int code) {
  String header;

  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial1.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial1.println("640x480");
  if (imgsize == VC0706_320x240) Serial1.println("320x240");
  if (imgsize == VC0706_160x120) Serial1.println("160x120");

  if (! cam.takePicture())
    Serial1.println("Failed to snap!");
  else
    Serial1.println("Picture taken!");

  // Get the size of the image (frame) taken
  uint16_t jpglen = cam.frameLength();
  Serial1.print("Sending ");
  Serial1.print(jpglen, DEC);
  Serial1.println(" byte image.");

  _prepareHeader(header, code, "image/jpeg", jpglen);
  _currentClient.write(header.c_str(), header.length());

  sendContentCam(jpglen);
  
  cam.cameraFrameBuffCtrl(VC0706_RESUMEFRAME);  // Adafruit lib needs this for esp8266
}

/***********************
   Camera code ends here
 ***********************/

