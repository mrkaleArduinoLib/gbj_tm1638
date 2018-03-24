/*
  NAME:
  Brigthness test of a display module with TM1638 controller and 7-segment
  digital tubes

  DESCRIPTION:
  The sketch turns on all glyph segments of all digital tubes and changes their
  brightness by controlling contrast.
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all 8 digital tubes with common cathode.
  - The sketch does not manipulate radix segments of digital tubes.
  - The sketch signals brightness level by red LEDs of the display module.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#define SKETCH "GBJ_TM1638_TEST_BRIGHTNESS 1.0.0"

const unsigned int PERIOD_TEST = 1000;  // Time in miliseconds between tests
const unsigned int PERIOD_PATTERN = 500; // Time delay in miliseconds for displaying a pattern
const unsigned char PIN_TM1638_CLK = 2;
const unsigned char PIN_TM1638_DIO = 3;
const unsigned char PIN_TM1638_STB = 4;

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
  Sled.printDigitOnAll();
}


void loop()
{
  if (Sled.isError()) return;
  // Test all contrast levels
  for (unsigned char contrast = 0; contrast < 8; contrast++)
  {
    Sled.printLedRedOffAll();
    Sled.printLedRedOn(contrast);
    Sled.setContrast(contrast);
    displayTest();
  }
  delay(PERIOD_TEST);
}
