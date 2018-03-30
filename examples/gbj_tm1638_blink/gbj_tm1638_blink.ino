/*
  NAME:
  Demo sketch for blinking all digital tubes and red LEDs of a display module
  with TM1638 controller.

  DESCRIPTION:
  The sketch turns on all 7 glyph segments of each digital tube including all
  red LEDs and blink them at once while the active radix segment at particular
  tube signals the contrast (brightness level).
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all 8 digital tubes and all 8 red LEDs
    with common cathode.
  - Because not all display modules are equipped with two-color LEDs, the green
    LEDs are not used in the sketch.
  - The digital tubes and LEDs are blinked several times (default 3) before
    changing contrast to the next level.
  - The sketch signals a contrast level by corresponding radix activated.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#define SKETCH "GBJ_TM1638_BLINK 1.0.0"

const unsigned int PERIOD_TEST = 1000;  // Time in miliseconds between tests
const unsigned int PERIOD_PATTERN = 500; // Time delay in miliseconds for displaying a pattern
const unsigned char PIN_TM1638_CLK = 2;
const unsigned char PIN_TM1638_DIO = 3;
const unsigned char PIN_TM1638_STB = 4;
const unsigned char TEST_BLINKS = 3;

gbj_tm1638 Sled = gbj_tm1638(PIN_TM1638_CLK, PIN_TM1638_DIO, PIN_TM1638_STB);


void errorHandler()
{
  if (Sled.isSuccess()) return;
  Serial.print("Error: ");
  Serial.println(Sled.getLastResult());
}


void displayTest()
{
  if (Sled.display()) errorHandler();
  delay(PERIOD_PATTERN);
}


void setup()
{
  Serial.begin(9600);
  Serial.println(SKETCH);
  Serial.println("Libraries:");
  Serial.println(GBJ_TM1638_VERSION);
  Serial.println("---");
  // Initialize controller
  if (Sled.begin())
  {
    errorHandler();
    return;
  }
  // Setup for test
  Sled.printDigitOn();
  Sled.printLedOnRed();
  Sled.displayOff();
}


void loop()
{
  if (Sled.isError()) return;
  // Cycle contrast
  for (unsigned char contrast = 0; contrast < 8; contrast++)
  {
    Sled.printRadixOff();
    Sled.setContrast(contrast);
    Sled.printRadixOn(contrast);
    // Cycle blinks
    for (unsigned char i = 0; i < TEST_BLINKS; i++)
    {
      Sled.displayOff();
      displayTest();
      Sled.displayOn();
      displayTest();
    }
  }
}
