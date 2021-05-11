#include <FastLED.h>
#include <stdlib.h>
#include <TFT_eSPI.h>
 
CRGB leds[150];
TFT_eSPI tft = TFT_eSPI();

int r = 255;
int b = 0;
int g = 0;
 
void setup() 
{
  FastLED.addLeds<WS2812, 25, GRB>(leds, 150);
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  tft.setTextSize(3);
}

void rock() {
//  blue = CRGB(0, 0, 100);
//  light_blue = CRGB(10, 10, 80);
//  lightest_blue = CRGB(20, 20, 60);
//  red = CRGB(100, 0, 0);
//  light_red = CRGB(80, 10, 10);
//  lightest_red = CRGB(60, 20, 20);
//  green = CRGB(0, 100, 0);
//  light_green = CRGB(10, 80, 10);
//  lightest_green = CRGB(20, 60, 20);
//  none = CRGB(0, 0, 0);
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setCursor(20, 20, 1);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("ROCK");

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/2;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(0, 0, 100);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(10, 10, 80);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(20, 20, 60);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = 3*weep/4;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(0, 0, 100);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(10, 10, 80);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(20, 20, 60);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }
  
  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(0, 0, 100);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(10, 10, 80);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(20, 20, 60);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/2;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(100, 0, 0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(80, 10, 10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(60, 20, 20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = 3*weep/4;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(100, 0, 0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(80, 10, 10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(60, 20, 20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(100, 0, 0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(80, 10, 10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(60, 20, 20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/2;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(0, 100, 0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(10, 80, 10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(20, 60, 20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = 3*weep/4;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(0, 100, 0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(10, 80, 10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(20, 60, 20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  for(int weep = 0; weep < 150; weep++)
  {
    int one = weep/1;
    for(int i = 0; i < one; i++)
    {
      if(i < 50)
      {
        leds[i] = CRGB(0, 100, 0);
      }
      else if(i < 100)
      {
        leds[i] = CRGB(10, 80, 10);
      }
      else if(i < 150)
      {
        leds[i] = CRGB(20, 60, 20);
      }
    }
    for(int ii = one; ii < 150; ii++)
    {
      leds[ii] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }
}


void noise() {
  tft.fillScreen(TFT_LIGHTGREY); //fill background
  tft.setCursor(20, 20, 1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("NOISE");
  
  if(r > 0 && b == 0){
    r--;
    g++;
  }
  if(g > 0 && r == 0){
    g--;
    b++;
  }
  if(b > 0 && g == 0){
    r++;
    b--;
  }

  for(int i = 0; i < 150; i++)
  {
    double r = ((double) rand() / (RAND_MAX));
    if (r > 0.5)
    {
      leds[i] = CRGB(r, g, b);
    }
    else
    {
      double rr = ((double) rand() / (RAND_MAX));
      if (rr > 0.5)
      {
        leds[i] = CRGB(100, 100, 100);
      }
      else
      {
        leds[i] = CRGB(0, 0, 0);
      }
    }
  }
  FastLED.show();
  
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
   rock();
//   noise();
//   pop();
}
