#include <Arduino.h>
#include <Wire.h>
#include <GxEPD2_3C.h>

#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define HOSTNAME "esp-epaper"

WiFiServer wifiServer(3333);
WiFiClient client;

GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(/*CS=D8*/ SS, /*DC=D3*/ D4, /*RST=D4*/ D3, /*BUSY=D2*/ D2));

void drawHelloWorld() {

  display.setFullWindow();
  display.firstPage();

  display.writeScreenBuffer();

  for (int y=0; y<300; y++ ){
    for (int x=0; x<400; x++ ){
      display.drawPixel(x, y, GxEPD_RED);
    }
  }

  display.nextPage();
  display.refresh();

}

void setup()
{
  WiFiManager wifiManager;

  //wifiManager.resetSettings();

  wifiManager.setConfigPortalTimeout(120);
  wifiManager.autoConnect(HOSTNAME);

  wifiServer.begin();

  ArduinoOTA.begin();

  //MDNS.begin(HOSTNAME);  // this doesn't work, wifiManager starts mDNS
  MDNS.setHostname(HOSTNAME);

  //ESP.wdtDisable();

}

bool clientConnected = false;
uint32_t pixel;

void loop() {
  ArduinoOTA.handle();

  //ESP.wdtFeed();

  if (WiFi.status() != WL_CONNECTED) {
    ESP.reset();
  }

  if (wifiServer.hasClient()) {
      client = wifiServer.available();
      clientConnected = true;
      pixel = 0;

      display.init();
      display.setFullWindow();
      display.firstPage();

      display.writeScreenBuffer();
  }

  if (client.connected()){
    if (int toRead = client.available()){
      if (toRead > 100){
        toRead = 100;
      }
      char buf[toRead];
      toRead = client.readBytes(buf, toRead);
      for (int i=0; i<toRead; i++){
        int col = GxEPD_WHITE;
        switch (buf[i]){
          case 0:
            col = GxEPD_WHITE;
            break;
          case 1:
            col = GxEPD_BLACK;
            break;
          case 2:
            col = GxEPD_RED;
            break;
        }
        display.drawPixel(pixel % display.width(), pixel / display.width(), col);
        pixel++;
      }
    }
  } else if (clientConnected) {
    clientConnected = false;
    display.nextPage();
    display.refresh();
  }



};
