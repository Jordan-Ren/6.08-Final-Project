#include <FastLED.h>
#include <stdlib.h>
#include <TFT_eSPI.h>
#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <mpu6050_esp32.h>
#include<math.h>
#include <ArduinoJson.h>

 
CRGB leds[150];
TFT_eSPI tft = TFT_eSPI();

int r = 70;
int b = 0;
int g = 0;

JsonArray genres;
double bpm = 100.0;
int loop_timer;

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

  lookup(response);
  loop_timer = millis();
}

void lookup(char* response) {
  char request_buffer[200];
  //CHANGE WHERE THIS IS TARGETED! IT SHOULD TARGET YOUR SERVER SCRIPT
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/team15/final/voice_text_input_with_secrets.py?group=%s HTTP/1.1\r\n", group);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body
  int response_size = 200;

  do_http_request("608dev-2.net", request_buffer, response, response_size, RESPONSE_TIMEOUT, true);

  char* begin_json=strchr(response,'{');
  char* end_json=strrchr(response,'}');

  end_json[1] = '\0';

  StaticJsonDocument<300> doc;
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, begin_json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  // Fetch values.
  //  char name[100] = {'\0'};
  bpm = doc["tempo"];
  Serial.print("bpm: ");
  Serial.println(bpm);
  if (bpm < 70.0) {
    bpm = 30.0;
  } else if (bpm > 150.0) {
    bpm = 300.0;
  }

  Serial.print("genre: ");
  genres = doc["genres"].as<JsonArray>();
  for (JsonVariant gee : genres) {
      Serial.print(gee.as<String>());
      Serial.print(", ");
  }
  Serial.println("");

  genre_setter(genres);
  
  loop_timer = millis();
}


void genre_setter(JsonArray genres) {
  for (JsonVariant gee : genres) {
      const char* text = gee.as<const char*>();
      char * popo;
      popo = strstr (text,"pop");
      char * rocko;
      rocko = strstr (text,"rock");
      char * lowo;
      lowo = strstr (text,"lo-fi");
      if (rocko!=NULL) {
        r = 0;
        g = 70;
        b = 0;
        Serial.println(text);
        break;
      } else if (popo!=NULL) {
        r = 70;
        g = 0;
        b = 0;
        Serial.println(text);
        break;
      } else if (lowo!=NULL) {
        r = 0;
        g = 10;
        b = 60;
        Serial.println(text);
        break;
      }
  }
}


void lightshow(JsonArray genres, double bpm) {
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setCursor(20, 20, 1);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
//  tft.println("ROCK");

  double time_pass = 60.0*1000.0/bpm;
  int start_timer = millis();
  
  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(r+0, g+0, b+0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(r+10, g+10, b+10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(r+20, g+20, b+20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  // !!!!!!!!!!!!!!!!!
  while ((millis() - start_timer) < time_pass) {
    FastLED.show();
  }
  start_timer = millis();
  

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(r+0, g+0, b+0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(r+10, g+10, b+10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(r+20, g+20, b+20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  // !!!!!!!!!!!!!!!!!
  while ((millis() - start_timer) < time_pass) {
    FastLED.show();
  }
  start_timer = millis();

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(r+0, g+0, b+0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(r+10, g+10, b+10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(r+20, g+20, b+20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  // !!!!!!!!!!!!!!!!!
  while ((millis() - start_timer) < time_pass) {
    FastLED.show();
  }

  
}

 
void loop() 
{
  if (millis() - loop_timer > 10000) {
    lookup(response);
  //  Serial.println(response);
  }
  
  lightshow(genres, bpm);
}
