#include <Arduino.h>
#include <Wire.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_BW.h>


#include <Adafruit_I2CDevice.h>

#if defined(ESP8266)
#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#endif

#if defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
//#include <ArduinoOTA.h>

#endif

#include "wifi_credentials.h"

//#include <ArduinoOTA.h>
//#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define HOSTNAME "epaper"

WiFiServer wifiServer(3333);
WiFiClient client;


/*
SPI connection https://components101.com/microcontrollers/esp32-devkitc

SPI SS - IO5 (D5)
SPI SCK - IO18 (D18)
SPI MISO - IO19
SPI MOSI - IO23
the rest of pins is in the constructor below
*/

#define EPAPER_750_T7
//#define EPAPER_420c

//GxEPD2_3C<GxEPD2_270c, GxEPD2_270c::HEIGHT> display(GxEPD2_270c(/*CS=*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

//GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(/*CS=*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

#ifdef EPAPER_420c
  GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(/*CS=*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
  #define DISPLAY_WIDTH 400
  #define DISPLAY_HEIGHT 300
#endif

#ifdef EPAPER_750_T7
  GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
  //#define DISPLAY_WIDTH 480
  //#define DISPLAY_HEIGHT 480
#endif

WiFiManager wifiManager;
/*
void drawHelloWorld() {

  display.setFullWindow();
  display.firstPage();

  display.writeScreenBuffer();

  for (int y=0; y < DISPLAY_HEIGHT; y++ ){
    for (int x=0; x < DISPLAY_WIDTH; x++ ){
      display.drawPixel(x, y, GxEPD_RED);
    }
  }

  display.nextPage();
  display.refresh();

}*/

RTC_DATA_ATTR int bootCount = 0;

void setup22()
{
  Serial.begin(115200);

  bootCount++;
  Serial.println("wakeup" + String(bootCount));

  pinMode(2, OUTPUT);
  
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);

  Serial.println("Going to sleep now");
  delay(200);
  Serial.flush(); 

  #define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
  #define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void setup()
{
  //wifiManager.resetSettings();

  //wifiManager.setConfigPortalTimeout(120);
  //wifiManager.autoConnect(HOSTNAME);

  Serial.begin(115200);

  bool res;
  res = wifiManager.autoConnect("epaper"); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }

  wifiServer.begin();
/*
  if (!MDNS.begin("epaper")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");
*/
  //ArduinoOTA.begin();

  //MDNS.begin(HOSTNAME);  // this doesn't work, wifiManager starts mDNS
  //MDNS.setInstanceName(HOSTNAME);

  //ESP.wdtDisable();


  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
/*
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
*/
}

bool clientConnected = false;
uint32_t pixel;

void loop22()
{

}

void loop() {
  //ArduinoOTA.handle();
  //wifiManager.process();


  //ESP.wdtFeed();
/*
  if (WiFi.status() != WL_CONNECTED) {
    //ESP.reset();
    //Serial.println("Connection error");
  }*/

  if (wifiServer.hasClient()) {
      Serial.println("Has client");

      client = wifiServer.available();
      clientConnected = true;
      pixel = 0;

      //display.init();
      display.init(115200, true, 2); // 7.5"
      Serial.println("Display init");
      
      display.setFullWindow();
      
      
      //display.setPartialWindow(0, 0, display.width(), display.height() / 2);
      display.firstPage();

      display.writeScreenBuffer();
  }

  if (client.connected()){
    Serial.printf("Client connected");
    if (int toRead = client.available()){
      Serial.printf("Client read tot %d", toRead);
      if (toRead > 100){
        toRead = 100;
      }
      char buf[toRead];
      toRead = client.readBytes(buf, toRead);
      Serial.printf("Client read %d", toRead);
      for (int i=0; i<toRead; i++){
        int color = GxEPD_WHITE;
        switch (buf[i]){
          case 0:
            color = GxEPD_WHITE;
            break;
          case 1:
            color = GxEPD_BLACK;
            break;
          case 2:
            color = GxEPD_RED;
            break;
        }
        int x = pixel % 480; // display.width();
        int y = pixel / 480; //display.width();

       /* // Switch to next page in half of the screen
        if(x == 0 && y == 240)
        {
              Serial.println("Next page");
              display.nextPage();
        }*/

        display.drawPixel(x, y, color);
        //Serial.printf("[%d,%d,%d],", x, y, color);
        pixel++;
      }
    }
  } else if (clientConnected) {
    Serial.printf("Pixels %d", pixel);
    clientConnected = false;
    display.nextPage();

    display.refresh();
  }



};
