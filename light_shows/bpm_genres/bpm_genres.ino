#include <FastLED.h>
#include <stdlib.h>
#include <TFT_eSPI.h>
#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <mpu6050_esp32.h>
#include<math.h>
 
CRGB leds[150];
TFT_eSPI tft = TFT_eSPI();

int r = 255;
int b = 0;
int g = 0;

char group[] = "test1";

MPU6050 imu; //imu object called, appropriately, imu

char network[] = "MIT GUEST";
char password[] = "";

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char old_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

uint32_t timer;
const int LOOP_PERIOD = 10000;
 
void setup() 
{
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  
  FastLED.addLeds<WS2812, 25, GRB>(leds, 150);
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  tft.setTextSize(3);
}

void lookup(char* response) {
  char request_buffer[200];
  //CHANGE WHERE THIS IS TARGETED! IT SHOULD TARGET YOUR SERVER SCRIPT
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/team15/final/voice_text_input_with_secrets.py?group=%s HTTP/1.1\r\n", group);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body
  int response_size = 200;

  do_http_request("608dev-2.net", request_buffer, response, response_size, RESPONSE_TIMEOUT, true);
}



void pop() {
  tft.fillScreen(TFT_WHITE); //fill background
  tft.setCursor(20, 20, 1);
  tft.setTextColor(TFT_PINK, TFT_PURPLE);
  tft.println("POP");

  for(int i = 0; i < 150; i++)
  {
    int r = rand() % 100;
    int g = rand() % 100;
    int b = rand() % 100;
    
    double rr = ((double) rand() / (RAND_MAX));
    if (rr > 0.25)
    {
      leds[i] = CRGB(r, g, b);
    }
    else
    {
      double rrr = ((double) rand() / (RAND_MAX));
      if (rrr > 0.5)
      {
        int rn = rand() % 100;
        int gn = rand() % 50;
        int bn = rand() % 50;
        leds[i] = CRGB(rn, gn, bn);
      }
      else
      {
        leds[i] = CRGB(0, 0, 0);
      }
    }
  }
  FastLED.show();
  usleep(500000); //half second, 120 bpm
}

 
void loop() 
{
//   rock();
//   noise();
   pop();
   lookup(response);
   Serial.println(response);
}
