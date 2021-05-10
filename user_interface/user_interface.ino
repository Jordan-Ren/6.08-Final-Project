#include <SPI.h>
#include <FastLED.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
//WiFiClientSecure is a big library. It can take a bit of time to do that first compile

const char* CA_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                      "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                      "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                      "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                      "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                      "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                      "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                      "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                      "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                      "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                      "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                      "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                      "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                      "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                      "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                      "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                      "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                      "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                      "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                      "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                      "-----END CERTIFICATE-----\n";



TFT_eSPI tft = TFT_eSPI();

const int DELAY = 1000;
const int SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const int SAMPLE_DURATION = 5;                        // duration of fixed sampling (seconds)
const int NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of of samples
const int ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip
const int LED = 25; // Change this as necessary
const int NUM_LEDS = 74; // Change this for the length of your LED strip
CRGB leds[NUM_LEDS];

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 3000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 3000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char host[] = "608dev-2.net";
char recorded_transcript[100] = {0};

/* CONSTANTS */
//Prefix to POST request:
const char PREFIX[] = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\", \"speechContexts\":[{\"phrases\":[\"play despacito\",\"pause\", \"resume\", \"skip\"]}]}, \"audio\": {\"content\":\"";
const char SUFFIX[] = "\"}}"; //suffix to POST request
const int AUDIO_IN = A0; //pin where microphone is connected
const char API_KEY[] = "AIzaSyCwyynsePu7xijUYTOgR7NdVqxH2FAG9DQ"; //don't change this


const uint8_t PIN_1 = 5; //button 1
const uint8_t PIN_2 = 19; //button 2

// STATES
const int IDLE = 0;
const int RECORDED = 1;
int state;


/* Global variables*/
uint8_t button_state; //used for containing button state and detecting edges
int old_button_state; //used for detecting button edges
uint32_t time_since_sample;      // used for microsecond timing


char speech_data[ENC_LEN + 200] = {0}; //global used for collecting speech data
const char* NETWORK     = "MIT";     // your network SSID (name of wifi network)
const char* PASSWORD = ""; // your network password
const char*  SERVER = "speech.google.com";  // Server URL

uint8_t old_val;
uint32_t timer;

WiFiClientSecure client; //global WiFiClient Secure object

void setup() {
  Serial.begin(115200);               // Set up serial port
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  delay(100); //wait a bit (100 ms)
  pinMode(PIN_1, INPUT_PULLUP);
  pinMode(PIN_2, INPUT_PULLUP);
  pinMode(25,OUTPUT); digitalWrite(25,0);//in case you're controlling your screen with pin 25

  WiFi.begin(NETWORK, PASSWORD); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
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
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  timer = millis();
  client.setCACert(CA_CERT); //set cert for https
  old_val = digitalRead(PIN_1);
  state = IDLE;
  tft.setCursor(0,0);
  tft.println("Hold button to record");
  FastLED.addLeds<WS2812, LED, GRB>(leds, NUM_LEDS);
}

//main body of code
void loop() {
  switch (state) {
    case IDLE:
      audio_control();
      break;
    case RECORDED:
      if (digitalRead(PIN_2) == 0) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0,0);
        tft.println("Sending Request...");
        send_request(recorded_transcript);
        state = IDLE;
      }
      if (digitalRead(PIN_1) == 0) {
        state = IDLE;
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0,0);
        tft.println("Hold button to record");
      }
      break;
  }
//  led_control();
  //pulse_to_bpm(100, 0, 0, 125);
}

void pulse_to_bpm(uint8_t r, uint8_t g, uint8_t b, int bpm) {
  int de = int((60.0/bpm)*1000*.85); // NOTE: TIMING STILL OFF FIX
  while (true) {
    fade_out(r, g, b);
    delay(de);
  }
}


void scroll_left(uint8_t r, uint8_t g, uint8_t b) {
  for (int i=0; i < NUM_LEDS;i++) {
    leds[i] = CRGB(r, g, b);
    FastLED.show();
  }
}

void scroll_right(uint8_t r, uint8_t g, uint8_t b) {
  for (int i=NUM_LEDS; i > 0;i--) {
    leds[i] = CRGB(r, g, b);
    FastLED.show();
  }
}

void set_all(uint8_t r, uint8_t g, uint8_t b) {
  for (int i=0; i<NUM_LEDS;i++) {
    leds[i] = CRGB(r, g, b);
  }
  FastLED.show();
}

void fade_out(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t start = micros();
  float red;
  float green;
  float blue;
  for (int i=255; i>0; i=i-8) {
    red = (i/256.0)*r;
    green = (i/256.0)*g;
    blue = (i/256.0)*b;
    set_all(int(red), int(green), int(blue));
  }
  set_all(0, 0, 0);
  uint32_t end_ = micros();
  //Serial.println(end_ - start);
}

void led_control() {
  for (int i = 0; i < NUM_LEDS-3; i = i+3) {
    leds[i] = CRGB(100, 0, 0);
    leds[i+1] = CRGB(0, 100, 0);
    leds[i+2] = CRGB(0, 0, 100);
    FastLED.show();
  }
}

void send_request(char * trans) {
  tft.fillScreen(TFT_BLACK);
  char body[100]; //I need to be changed.
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team15/final/voice_text_input.py HTTP/1.1\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Host: %s\r\n", host);
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(body, "user=acelli&group=test1&password=pass1&voice=%s", trans);
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n\r\n", strlen(body));
  strcat(request_buffer, body);
  do_http_request(host, request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);
  tft.setCursor(0,0);
  tft.println(response_buffer);
}

//function used to record audio at sample rate for a fixed nmber of samples
void record_audio() {
  int sample_num = 0;    // counter for samples
  int enc_index = strlen(PREFIX) - 1;  // index counter for encoded samples
  float time_between_samples = 1000000 / SAMPLE_FREQ;
  int value = 0;
  char raw_samples[3];   // 8-bit raw sample data array
  memset(speech_data, 0, sizeof(speech_data));
  sprintf(speech_data, "%s", PREFIX);
  char holder[5] = {0};
  Serial.println("starting");
  uint32_t text_index = enc_index;
  uint32_t start = millis();
  time_since_sample = micros();
  while (sample_num < NUM_SAMPLES && digitalRead(PIN_1) == 0) { //read in NUM_SAMPLES worth of audio data
    value = analogRead(AUDIO_IN);  //make measurement
    raw_samples[sample_num % 3] = mulaw_encode(value - 1551); //remove 1.25V offset (from 12 bit reading)
    sample_num++;
    if (sample_num % 3 == 0) {
      base64_encode(holder, raw_samples, 3);
      strncat(speech_data + text_index, holder, 4);
      text_index += 4;
    }
    // wait till next time to read
    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
  }
  Serial.println(millis() - start);
  sprintf(speech_data + strlen(speech_data), "%s", SUFFIX);
  Serial.println("out");
}

void audio_control() {
  button_state = digitalRead(PIN_1);
  if (!button_state && button_state != old_button_state) {
    Serial.println("listening...");
    record_audio();
    Serial.println("sending...");
    Serial.print("\nStarting connection to server...");
    delay(300);
    bool conn = false;
    for (int i = 0; i < 10; i++) {
      int val = (int)client.connect(SERVER, 443);
      Serial.print(i); Serial.print(": "); Serial.println(val);
      if (val != 0) {
        conn = true;
        break;
      }
      Serial.print(".");
      delay(300);
    }
    if (!conn) {
      Serial.println("Connection failed!");
      return;
    } else {
      Serial.println("Connected to server!");
      Serial.println(client.connected());
      int len = strlen(speech_data);
      // Make a HTTP request:
      client.print("POST /v1/speech:recognize?key="); client.print(API_KEY); client.print(" HTTP/1.1\r\n");
      client.print("Host: speech.googleapis.com\r\n");
      client.print("Content-Type: application/json\r\n");
      client.print("cache-control: no-cache\r\n");
      client.print("Content-Length: "); client.print(len);
      client.print("\r\n\r\n");
      int ind = 0;
      int jump_size = 1000;
      char temp_holder[jump_size + 10] = {0};
      Serial.println("sending data");
      while (ind < len) {
        delay(80);//experiment with this number!
        //if (ind + jump_size < len) client.print(speech_data.substring(ind, ind + jump_size));
        strncat(temp_holder, speech_data + ind, jump_size);
        client.print(temp_holder);
        ind += jump_size;
        memset(temp_holder, 0, sizeof(temp_holder));
      }
      client.print("\r\n");
      //Serial.print("\r\n\r\n");
      Serial.println("Through send...");
      unsigned long count = millis();
      while (client.connected()) {
        Serial.println("IN!");
        String line = client.readStringUntil('\n');
        Serial.print(line);
        if (line == "\r") { //got header of response
          Serial.println("headers received");
          break;
        }
        if (millis() - count > RESPONSE_TIMEOUT) break;
      }
      Serial.println("");
      Serial.println("Response...");
      count = millis();
      while (!client.available()) {
        delay(100);
        Serial.print(".");
        if (millis() - count > RESPONSE_TIMEOUT) break;
      }
      Serial.println();
      Serial.println("-----------");
      memset(response, 0, sizeof(response));
      while (client.available()) {
        char_append(response, client.read(), OUT_BUFFER_SIZE);
      }
      Serial.println(response);
      char* trans_id = strstr(response, "transcript");
      tft.setCursor(0,0);
      if (trans_id != NULL) {
        char* foll_coll = strstr(trans_id, ":");
        char* starto = foll_coll + 2; //starting index
        char* endo = strstr(starto + 1, "\""); //ending index
        int transcript_len = endo - starto + 1;
        char transcript[100] = {0};
        strncat(transcript, starto, transcript_len);
        for(int i = 0; i < sizeof(transcript); i++)
          transcript[i] = tolower(transcript[i]);
        Serial.println(transcript);
        state = RECORDED;
        tft.fillScreen(TFT_BLACK);
        tft.println("Current Action:");
        tft.println(transcript);
        sprintf(recorded_transcript, transcript);
      } else {
        tft.fillScreen(TFT_BLACK);
        tft.println("Unable to transcribe.");
        tft.println("Please re-record audio");
      }
      Serial.println("-----------");
      client.stop();
      Serial.println("done");
    }
  }
  old_button_state = button_state;
}
