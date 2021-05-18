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
double old_bpm = 100.0;
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
  old_bpm = bpm;
  bpm = doc["tempo"];
  Serial.print("bpm: ");
  Serial.println(bpm);

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
      char * pop;
      pop = strstr (text,"pop");
      char * rock;
      rock = strstr (text,"rock");
      char * low;
      low = strstr (text,"lo-fi");
      char * indie;
      indie = strstr (text,"indie");
      char * folk;
      folk = strstr (text,"folk");
      char * latin;
      latin = strstr (text,"latin");
      char * metal;
      metal = strstr (text,"metal");
      char * jazz;
      jazz = strstr (text,"jazz");
      char * rap;
      rap = strstr (text,"rap");
      if (rock!=NULL) {
        r = 0;
        g = 70;
        b = 0;
        Serial.println(text);
        break;
      } else if (pop!=NULL) {
        r = 70;
        g = 0;
        b = 0;
        Serial.println(text);
        break;
      } else if (low!=NULL) {
        r = 0;
        g = 10;
        b = 60;
        Serial.println(text);
        break;
      } else if (indie!=NULL) {
        r = 0;
        g = 0;
        b = 10;
        Serial.println(text);
        break;
      } else if (folk!=NULL) {
        r = 10;
        g = 50;
        b = 0;
        Serial.println(text);
        break;
      } else if (latin!=NULL) {
        r = 50;
        g = 30;
        b = 0;
        Serial.println(text);
        break;
      } else if (metal!=NULL) {
        r = 5;
        g = 5;
        b = 5;
        Serial.println(text);
        break;
      } else if (jazz!=NULL) {
        r = 50;
        g = 0;
        b = 30;
        Serial.println(text);
        break;
      } else if (rap!=NULL) {
        r = 50;
        g = 50;
        b = 10;
        Serial.println(text);
        break;
      } else {
        int char_text = int(text);
        r = (char_text / 3) % 100;
        g = (char_text / 5) % 100;
        b = (char_text / 7) % 100;
      }
  }
}

bool bidirectional = true;
double grad = 0.1;
int maxi = 150;
void lightshow(JsonArray genres, double bpm) {
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setCursor(20, 20, 1);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);

  double time_pass = min(60.0*1000.0/bpm, 5000);
  int start_timer = millis();
  
  for(int weep = 0; weep < maxi; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      leds[i] = CRGB(int(r+(grad*i)), int(g+(grad*i)), int(b+(grad*i)));
      if (bidirectional) {
        leds[150 - i] = CRGB(int(r+(grad*i)), int(g+(grad*i)), int(b+(grad*i)));
      }
    }
    for(int ii = one; ii < maxi; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
      if (bidirectional) {
        leds[150 - ii] = CRGB(0, 0, 0);
      }
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
    if (old_bpm != bpm) {
      for(int y = 0; y < 150; y++)
      {
        leds[y] = CRGB(rand() % 40, rand() % 10, rand() % 10); // 0,0,0
      }
      FastLED.show();
      maxi = (int) 150 * (bpm / 300);
    } else {
      for(int y = maxi; y < 150; y++)
      {
        double rrr = ((double) rand() / (RAND_MAX));
        if (rrr > 0.8)
        {
          leds[y] = CRGB(r + rand() % 10, g + rand() % 5, b + rand() % 5); // 0,0,0
        }
        else
        {
          leds[y] = CRGB(0, 0, 0);
        }
      }
      FastLED.show();
    }
  //  Serial.println(response);
    loop_timer = millis();
  }
  
  lightshow(genres, bpm);
}
