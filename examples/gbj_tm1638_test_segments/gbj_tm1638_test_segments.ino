/*
  NAME:
  Digit segments functionality test for a display module with TM1638 controller
  and 7-segment digital tubes

  DESCRIPTION:
  The sketch tests all 7 segments of each digital tube displaying them one by one
  on each tube one by one.
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all 8 digital tubes with common cathode.
  - The sketch does not manipulate radix segments of digital tubes.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#define SKETCH "GBJ_TM1638_TEST_SEGMENTS 1.0.0"

const unsigned int PERIOD_TEST = 1000;  // Time in miliseconds between tests
const unsigned int PERIOD_PATTERN = 300; // Time delay in miliseconds for displaying a pattern
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
}


void loop()
{
  if (Sled.isError()) return;
  Sled.printGridOffAll();
  // Test all digits one by one
  for (unsigned char grid = 0; grid < Sled.getGrids(); grid++)
  {
    // Display segments one by one of a digit
    for (unsigned char segment = 0; segment < 7; segment++)
    {
      Sled.printGrid(grid, 0x01 << segment);
      displayTest();
    }
    // Display all segments of a digit
      Sled.printGrid(grid);
      displayTest();
  }
  delay(PERIOD_TEST);
}
