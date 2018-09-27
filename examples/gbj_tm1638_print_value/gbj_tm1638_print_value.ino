/*
  NAME:
  Demo of text and number printing with the library gbj_tm1638

  DESCRIPTION:
  The sketch prints some informative texts on the display and count down and up.
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all 8 digital tubes with common cathode.
  - The sketch utilizes basic font and prints with system functions.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#include "../extras/font7seg_basic.h"
#define SKETCH "GBJ_TM1638_PRINT_TIME 1.0.0"

const unsigned int PERIOD_TEST = 2000;  // Time in miliseconds between tests
const unsigned int PERIOD_VALUE = 100; // Time delay in miliseconds for displaying a value
const unsigned char PIN_TM1638_CLK = 2;
const unsigned char PIN_TM1638_DIO = 3;
const unsigned char PIN_TM1638_STB = 4;
const unsigned int TEST_LIMIT = 150;

gbj_tm1638 Sled = gbj_tm1638(PIN_TM1638_CLK, PIN_TM1638_DIO, PIN_TM1638_STB);


void errorHandler()
{
  if (Sled.isSuccess()) return;
  Serial.print("Error: ");
  Serial.println(Sled.getLastResult());
  Serial.println(Sled.getLastCommand());
}


void displayTest()
{
  if (Sled.display()) errorHandler();
  delay(PERIOD_TEST);
}


void setup()
{
  Serial.begin(9600);
  Serial.println(SKETCH);
  Serial.println("Libraries:");
  Serial.println(gbj_tm1638::VERSION);
  Serial.println("Fonts:");
  Serial.println(GBJ_FONT7SEG_VERSION);
  Serial.println("---");
  // Initialize controller
  Sled.begin();
  if (Sled.isError())
  {
    errorHandler();
    return;
  }

  Sled.setFont(gbjFont7segTable, sizeof(gbjFont7segTable));
  if (Sled.isError())
  {
    errorHandler();
    return;
  }
}


void loop()
{
  if (Sled.isError()) return;
  Sled.printText("Init");
  displayTest();
  for (unsigned int i = 0; i < TEST_LIMIT; i++)
  {
    Sled.displayClear();
    Sled.print(i / 100.0, 2);
    Sled.placePrint(4);
    Sled.print((TEST_LIMIT - i) / 100.0, 2);
    Sled.display();
    delay(PERIOD_VALUE);
  }
  Sled.printText("End", 5);
  displayTest();
}
