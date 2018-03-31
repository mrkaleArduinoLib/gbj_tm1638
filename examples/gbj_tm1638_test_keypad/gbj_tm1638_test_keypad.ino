/*
  NAME:
  Demo of keypad pressing processing with the library gbj_tm1638

  DESCRIPTION:
  The sketch performs some visual effects for various click actions on a key and
  prints particular action name to the serial monitor.
  - Connect controller's pins to Arduino's pins as follows:
    - TM1638 pin CLK to Arduino pin D2
    - TM1638 pin DIO to Arduino pin D3
    - TM1638 pin STB to Arduino pin D4
    - TM1638 pin Vcc to Arduino pin 5V
    - TM1638 pin GND to Arduino pin GND
  - The sketch is configured to work with all hardware elements, i.e., 8 digital
    tubes, 8 red LEDs, and 8 keys.
  - By single clicking on a key, the sketch turns on corresponding red LED.
  - By double clicking on a key, the sketch turns off corresponding red LED.
  - By single holding a key pressed, the sketch turns on corresponding tube.
  - By double holding a key pressed, the sketch turns of corresponding tube.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_tm1638.h"
#define SKETCH "GBJ_TM1638_KEYPAD 1.0.0"

const unsigned int PERIOD_TEST = 2000;  // Time in miliseconds between tests
const unsigned int PERIOD_VALUE = 300; // Time delay in miliseconds for displaying a value
const unsigned char PIN_TM1638_CLK = 2;
const unsigned char PIN_TM1638_DIO = 3;
const unsigned char PIN_TM1638_STB = 4;

gbj_tm1638 Sled = gbj_tm1638(PIN_TM1638_CLK, PIN_TM1638_DIO, PIN_TM1638_STB);


void errorHandler()
{
  if (Sled.isSuccess()) return;
  Serial.print("Error: ");
  Serial.println(Sled.getLastResult());
  Serial.println(Sled.getLastCommand());
}


void keyHandler(uint8_t key, uint8_t action)
{
  Serial.print("key S");
  Serial.print(key + 1);
  Serial.print(": ");
  switch (action)
  {
    case GBJ_TM1638_KEY_CLICK:
      Serial.println("GBJ_TM1638_KEY_CLICK");
      Sled.printLedOnRed(key);
      break;

    case GBJ_TM1638_KEY_HOLD:
      Serial.println("GBJ_TM1638_KEY_HOLD");
      Sled.printDigitOn(key);
      break;

    case GBJ_TM1638_KEY_CLICK_DOUBLE:
      Serial.println("GBJ_TM1638_KEY_CLICK_DOUBLE");
      Sled.printLedOff(key);
      break;

    case GBJ_TM1638_KEY_HOLD_DOUBLE:
      Serial.println("GBJ_TM1638_KEY_HOLD_DOUBLE");
      Sled.printDigitOff(key);
      break;
  }
  if (Sled.display()) errorHandler();
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
  Serial.println("---");
  Serial.println("Waiting for keypad...");
  // Initialize controller
  Sled.begin();
  if (Sled.isError())
  {
    errorHandler();
    return;
  }
  Sled.registerHandler(keyHandler);
}


void loop()
{
  if (Sled.isError()) return;
  Sled.run();
}
