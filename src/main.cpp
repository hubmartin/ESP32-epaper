#include <Arduino.h>
#include <Wire.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>


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

#endif

#include "wifi_credentials.h"

//#include <ArduinoOTA.h>
//#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define HOSTNAME "epaper"

WiFiServer wifiServer(3333);
WiFiClient client;

void drawStartScreen();

/*
SPI connection https://components101.com/microcontrollers/esp32-devkitc
SPI SS - IO5 (D5)
SPI SCK - IO18 (D18)
SPI MISO - IO19
SPI MOSI - IO23
the rest of pins is in the constructor below
*/

// Uncomment single display you use
//#define EPAPER_270c
//#define EPAPER_420c
#define EPAPER_750_T7

// Uncomment if you're using Waveshare E-Paper ESP32 Driver Board (ESP32 board with Epaper connector)
//#define WAVESHARE_ALTERNATE_PINS


#ifdef WAVESHARE_ALTERNATE_PINS
  #define PINS /*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25
#else
  #define PINS /*CS=*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4
#endif


#ifdef EPAPER_270c
  // 176x264
  GxEPD2_3C<GxEPD2_270c, GxEPD2_270c::HEIGHT> display(GxEPD2_270c(PINS));
#endif

#ifdef EPAPER_420c
  // 400x300
  GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display(GxEPD2_420c(PINS));
#endif

#ifdef EPAPER_750_T7
  // 800x480
  GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(PINS));
#endif


WiFiManager wifiManager;
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
void setup()
{
  //wifiManager.resetSettings();
  //wifiManager.setConfigPortalTimeout(120);
  //wifiManager.autoConnect(HOSTNAME);

  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

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

  #ifdef EPAPER_750_T7
  display.init(115200, true, 2); // 7.5"
  #else
  display.init();
  #endif

  #ifdef WAVESHARE_ALTERNATE_PINS
  // *** special handling for Waveshare ESP32 Driver board *** //
  // ********************************************************* //
  SPI.end(); // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  //SPI: void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
  SPI.begin(13, 12, 14, 15); // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
  // *** end of special handling for Waveshare ESP32 Driver board *** //
  // **************************************************************** //
  #endif

  Serial.printf("Display init width: %d", display.width());

  drawStartScreen();
 
}

bool clientConnected = false;
uint32_t pixel;

void loop() {
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
      
      display.setFullWindow();
            
      //display.setPartialWindow(0, 0, display.width(), display.height() / 2);
      display.firstPage();

      display.writeScreenBuffer();
  }

  if (client.connected()){
    //Serial.printf("Client connected");
    if (int toRead = client.available()){
      //Serial.printf("Client read tot %d", toRead);
      if (toRead > 100){
        toRead = 100;
      }
      char buf[toRead];
      toRead = client.readBytes(buf, toRead);
      //Serial.printf("Client read %d", toRead);
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
        int x = pixel % display.width();
        int y = pixel / display.width();

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





// Low power tests

RTC_DATA_ATTR int bootCount = 0;

void deep_sleep_setup()
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





void drawStartScreen()
{
  char bootText[50];
  char ipText[20];

  IPAddress ip = WiFi.localIP();
  ip.toString().toCharArray(ipText, 16);

  snprintf(bootText, sizeof(bootText), "IP %s", ipText);
  
  //Serial.println("helloWorld");
  //display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(bootText, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(bootText);
  }
  while (display.nextPage());
  //Serial.println("helloWorld done");
}