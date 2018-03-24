/*
  NAME:
  Demo of scrollilng text with the library gbj_tm1638

  DESCRIPTION:
  The sketch prints some informative texts on the display and uptime for 1 minute.
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
#define SKETCH "GBJ_TM1638_SCROLL 1.0.0"

const unsigned int PERIOD_TEST = 2000;  // Time in miliseconds between tests
const unsigned int PERIOD_VALUE = 300; // Time delay in miliseconds for displaying a value
const unsigned char PIN_TM1638_CLK = 2;
const unsigned char PIN_TM1638_DIO = 3;
const unsigned char PIN_TM1638_STB = 4;

gbj_tm1638 Sled = gbj_tm1638(PIN_TM1638_CLK, PIN_TM1638_DIO, PIN_TM1638_STB);
String textScroll = "0123456789-0123456789";
String textBuffer;


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
  Serial.println(GBJ_TM1638_VERSION);
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

  // Prepend leading spaces to scroling text
  textBuffer.reserve(textScroll.length() + ((Sled.getDigits() - 1) * 2));
  for (unsigned char i = 0; i < Sled.getDigits() - 1; i++)
  {
    textBuffer += ' ';
  }
  textBuffer += textScroll;
}


void loop()
{
  if (Sled.isError()) return;
  for (unsigned char i = 0; i < textBuffer.length(); i++)
  {
    // Sled.displayClear();
    Sled.printText(textBuffer.substring(i));
    Sled.display();
    delay(PERIOD_VALUE);
  }
}
