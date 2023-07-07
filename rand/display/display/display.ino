//despite what any guide says - if you don't plug in the rst pin to a reset port, it will not work

#include <arduino.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 10
#define TFT_CS 7
#define TFT_DIN 8
#define TFT_CLK 9

// backlight will eventually go in GPIO breakout board

//hardware SPI -- will likely have to change pins for the motor shield - grrr
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_DIN, TFT_CLK); //change constructor to pass in more pin params

void setup() {
  //start serial
  Serial.begin(9600);
  Serial.println("Starting display");

  //bring up tst
  tft.begin();
  
}

void loop() {
  tft.setTextColor(ILI9341_BLUE);
  tft.setCursor(0,0);
  tft.println("Testing!");
  delay(1000);

  tft.fillScreen(ILI9341_WHITE);
  delay(1000);

}
