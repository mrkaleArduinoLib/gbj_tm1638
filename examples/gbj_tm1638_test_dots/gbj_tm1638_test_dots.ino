/*
  NAME:
  Radix segments functionality test for a display module with TM1638

  DESCRIPTION:
  The sketch tests all radixes of a display module.
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all 8 digital tubes with common cathode.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#define SKETCH "GBJ_TM1638_TEST_RADIXES 1.0.0"

const unsigned int PERIOD_TEST = 500;  // Time in miliseconds between tests
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
  delay(PERIOD_TEST);
}


void setup()
{
  Serial.begin(9600);
  Serial.println(SKETCH);
  Serial.println("Libraries:");
  Serial.println(gbj_tm1638::VERSION);
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
  // Display radixes one by one
  for (unsigned char digit = 0; digit < Sled.getDigits(); digit++)
  {
    Sled.printRadixOn(digit);
    displayTest();
    Sled.printRadixOff(digit);
  }
  for (signed char digit = Sled.getDigits() - 2; digit >= 0; digit--)
  {
    Sled.printRadixOn(digit);
    displayTest();
    Sled.printRadixOff(digit);
  }
  displayTest();
  // Display radixes as thermometer
  for (unsigned char digit = 0; digit < Sled.getDigits(); digit++)
  {
    Sled.printRadixOn(digit);
    displayTest();
  }
  for (signed char digit = Sled.getDigits() - 1; digit >=0; digit--)
  {
    Sled.printRadixOff(digit);
    displayTest();
  }
}
