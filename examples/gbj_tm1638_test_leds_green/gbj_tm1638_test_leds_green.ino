/*
  NAME:
  Green LEDs functionality test for a display module with TM1638 controller

  DESCRIPTION:
  The sketch tests all red LEDs of a display module.
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all 8 green LEDs with common cathode.
  - Some display modules are not equipped with two-color LEDs, so that do not
    use this sketch with modules just with red LEDs.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#define SKETCH "GBJ_TM1638_TEST_LEDS_RED 1.0.0"

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
  // Display LEDs one by one
  for (unsigned char led = 0; led < Sled.getLeds(); led++)
  {
    Sled.printLedOnGreen(led);
    displayTest();
    Sled.printLedOff(led);
  }
  for (signed char led = Sled.getLeds() - 2; led >= 0; led--)
  {
    Sled.printLedOnGreen(led);
    displayTest();
    Sled.printLedOff(led);
  }
  displayTest();
  // Display LEDs as thermometer
  for (unsigned char led = 0; led < Sled.getLeds(); led++)
  {
    Sled.printLedOnGreen(led);
    displayTest();
  }
  for (signed char led = Sled.getLeds() - 1; led >=0; led--)
  {
    Sled.printLedOff(led);
    displayTest();
  }
}
