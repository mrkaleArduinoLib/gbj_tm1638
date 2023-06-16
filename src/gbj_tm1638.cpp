#include "gbj_tm1638.h"
const String gbj_tm1638::VERSION = "GBJ_TM1638 1.0.0";


gbj_tm1638::gbj_tm1638(uint8_t pinClk, uint8_t pinDio, uint8_t pinStb, \
  uint8_t digits, uint8_t leds, uint8_t keys)
{
  status_.pinClk = pinClk;
  status_.pinDio = pinDio;
  status_.pinStb = pinStb;
  status_.digits = min(digits, getDigitsMax());
  status_.leds = min(leds, getLedsMax());
  status_.keys = min(keys, getKeysMaxHw());
}


uint8_t gbj_tm1638::begin()
{
  initLastResult();
  // Check pin duplicity
  if (status_.pinClk == status_.pinDio \
  ||  status_.pinDio == status_.pinStb \
  ||  status_.pinStb == status_.pinClk) return setLastResult(ERROR_PINS);
  // Setup pins
  pinMode(status_.pinClk, OUTPUT);
  pinMode(status_.pinDio, OUTPUT);
  pinMode(status_.pinStb, OUTPUT);
  // Initialize controller
  return setContrast();
}


//------------------------------------------------------------------------------
// Software manipulation - updating screen buffer
//------------------------------------------------------------------------------
// Print one character determined by a byte of ASCII code
size_t gbj_tm1638::write(uint8_t ascii)
{
  if (print_.digit >= status_.digits) return 0;
  uint8_t mask = getFontMask(ascii);
  if (mask == FONT_MASK_WRONG)
  {
    if (String(".,:").indexOf(ascii) >= 0)  // Detect radix
    {
      printRadixOn(print_.digit - 1); // Set radix to the previous digit
    }
    return 0;
  }
  else
  {
    printDigit(print_.digit, mask);
    return 1;
  }
}


// Print null terminated character array
size_t  gbj_tm1638::write(const char* text)
{
  uint8_t digits = 0;
  uint8_t i = 0;
  while (text[i] != '\0' && print_.digit < status_.digits)
  {
    digits += write(text[i++]);
  }
  return digits;
}


// Print byte array with length
size_t  gbj_tm1638::write(const uint8_t* buffer, size_t size)
{
  uint8_t digits = 0;
  for (uint8_t i = 0; i < size && print_.digit < status_.digits; i++)
  {
    digits += write(buffer[i]);
  }
  return digits;
}


//------------------------------------------------------------------------------
// Hardware manipulation - communication with the controller
//------------------------------------------------------------------------------
uint8_t gbj_tm1638::display()
{
  // Needed bytes in the buffer
  uint8_t bufferLen = max(getDigits(), getLeds()) * 2;
  if (getDigits() > getLeds()) bufferLen--;
  // Automatic addressing
  if (busSend(CMD_DATA_INIT | CMD_DATA_NORMAL | CMD_DATA_WRITE | CMD_DATA_AUTO)) return getLastResult();
  if (busSend(CMD_ADDR_INIT, print_.buffer, bufferLen)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_tm1638::displayOn()
{
  return setContrast(status_.contrast);
}


uint8_t gbj_tm1638::displayOff()
{
  return busSend(CMD_DISP_INIT | CMD_DISP_OFF);
}


//------------------------------------------------------------------------------
// Keypad processing
//------------------------------------------------------------------------------
void gbj_tm1638::registerHandler(gbj_tm1638_handler handler)
{
  keyProcesing_ = handler;
}


void gbj_tm1638::run()
{
  if (status_.keys == 0) return; // No key processing when no key is enabled
  uint32_t tsNow = millis();
  if (tsNow - status_.scanTimestamp >= TIMING_SCAN)
  {
    status_.scanTimestamp = tsNow;
    processKeypad();
  }
}

//------------------------------------------------------------------------------
// Setters
//------------------------------------------------------------------------------
uint8_t gbj_tm1638::setContrast(uint8_t contrast)
{
  status_.contrast = contrast & getContrastMax();
  return busSend(CMD_DISP_INIT | CMD_DISP_ON | status_.contrast);
}


void gbj_tm1638::setFont(const uint8_t* fontTable, uint8_t fontTableSize)
{
  font_.table = fontTable;
  font_.glyphs = fontTableSize / FONT_WIDTH;
}


//------------------------------------------------------------------------------
// Private methods
//------------------------------------------------------------------------------
// Wait for delay period expiry
void gbj_tm1638::waitPulseClk()
{
  delayMicroseconds(TIMING_RELAX);
}


// Start condition - pull down STB from HIGH to LOW
void gbj_tm1638::beginTransmission()
{
  digitalWrite(status_.pinStb, HIGH); // Finish previous communication for sure
  digitalWrite(status_.pinClk, HIGH);
  digitalWrite(status_.pinStb, LOW); // Start communication
}


// Stop condition - pull up STB from LOW to HIGH
void gbj_tm1638::endTransmission()
{
  digitalWrite(status_.pinStb, HIGH);
}


void gbj_tm1638::busWrite(uint8_t data)
{
  digitalWrite(status_.pinClk, LOW); // For active rising edge of clock pulse
  shiftOut(status_.pinDio, status_.pinClk, LSBFIRST, data);
}


uint8_t gbj_tm1638::busRead()
{
  digitalWrite(status_.pinClk, LOW); // For active rising edge of clock pulse
  uint8_t data = shiftIn(status_.pinDio, status_.pinClk, LSBFIRST);
  return data;
}


uint8_t gbj_tm1638::busSend(uint8_t command)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  endTransmission();
  return getLastResult();
}


uint8_t gbj_tm1638::busSend(uint8_t command, uint8_t data)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  busWrite(data);
  endTransmission();
  return getLastResult();
}


uint8_t gbj_tm1638::busSend(uint8_t command, uint8_t* buffer, uint8_t bufferItems)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  for (uint8_t bufferIndex = 0; bufferIndex < bufferItems; bufferIndex++)
  {
    busWrite(*buffer++);
  }
  endTransmission();
  return getLastResult();
}


uint8_t gbj_tm1638::busReceive(uint8_t command, uint8_t* buffer)
{
  beginTransmission();
  busWrite(setLastCommand(command));
  // Read bytes
  pinMode(status_.pinDio, INPUT);
  for (uint8_t bufferIndex = 0; bufferIndex < BYTES_SCAN; bufferIndex++)
  {
    buffer[bufferIndex] = busRead();
  }
  pinMode(status_.pinDio, OUTPUT);
  endTransmission();
  return getLastResult();
}


// The method leaves digit cursor after last print digit
void gbj_tm1638::gridWrite(uint8_t segmentMask, uint8_t gridStart, uint8_t gridStop)
{
  swapByte(gridStart, gridStop);
  gridStop = min(gridStop, status_.digits - 1);
  for (print_.digit = gridStart; print_.digit <= gridStop; print_.digit++)
  {
    segmentMask &= 0x7F; // Clear radix bit in segment mask
    print_.buffer[addrGrid(print_.digit)] &= 0x80;  // Clear digit bits in screen buffer
    print_.buffer[addrGrid(print_.digit)] |= segmentMask;  // Set digit bits but leave radix bit intact
  }
}


uint8_t gbj_tm1638::getFontMask(uint8_t ascii)
{
  uint8_t mask = FONT_MASK_WRONG;
  for (uint8_t glyph = 0; glyph < font_.glyphs; glyph++)
  {
    if (ascii == pgm_read_byte(&font_.table[glyph * FONT_WIDTH + FONT_INDEX_ASCII]))
    {
      mask = pgm_read_byte(&font_.table[glyph*2 + FONT_INDEX_MASK]);
      mask &= 0x7F; // Clear radix bit not to mess with wrong mask
      break;
    }
  }
  return mask;
}

/*
    Mapping of hardware switches to controller's keys
    S1 - K3/KS1 - BYTE1
    S2 - K3/KS3 - BYTE2
    S3 - K3/KS5 - BYTE3
    S4 - K3/KS7 - BYTE4
    S5 - K3/KS2 - BYTE1
    S6 - K3/KS4 - BYTE2
    S7 - K3/KS6 - BYTE3
    S8 - K3/KS8 - BYTE4
*/
uint8_t gbj_tm1638::processKeypad()
{
  uint8_t buffer[BYTES_SCAN];
  // Read all possible keys including not hardware implemented
  if (busReceive(CMD_DATA_INIT | CMD_DATA_NORMAL | CMD_DATA_READ, buffer)) return getLastResult();
  // Scan buses from K3 in descending order according to the datasheet
  for (uint8_t bus = 0; bus < status_.keys / 8 + 1; bus++)
  {
    uint8_t keyMask = 0;
    uint8_t bitMask = 0b10001 << bus; // Scanned keys for a bus
    for (uint8_t scanByte = 0; scanByte < sizeof(buffer) / sizeof(buffer[0]); scanByte++)
    {
      keyMask |= ((buffer[scanByte] & bitMask) >> bus) << scanByte; // Shift bus bit to first position
    }
    // Process keys in a bus
    for (uint8_t keyBit = 0; keyBit < 8; keyBit++)
    {
      uint8_t key = keyBit + (8 * bus);
      if (key >= status_.keys) break;
      bool keyPressed = keyMask & (1 << keyBit);
      uint8_t keyState;
      // Determine current key state
      if (keyPressed)
      {
        keys_[key].waitScans = 0;
        keyState = KEY_PRESS_SHORT;
        if (keys_[key].pressScans < 255) keys_[key].pressScans++;
        if (keys_[key].pressScans >= TIMING_SCAN_TRESHOLD_PRESS_LONG) keyState = KEY_PRESS_LONG;
      }
      else
      {
        keys_[key].pressScans = 0;
        keyState = KEY_WAIT_SHORT;
        if (keys_[key].waitScans < 255) keys_[key].waitScans++;
        if (keys_[key].waitScans >= TIMING_SCAN_TRESHOLD_WAIT) keyState = KEY_WAIT_LONG;
      }
      // Process action if state has changed
      if (keys_[key].keyState[0] != keyState)
      {
        // Historize key states
        for (uint8_t i = sizeof(keys_[key].keyState) / sizeof(keys_[key].keyState[0]) - 1; i > 0; i--)
        {
          keys_[key].keyState[i] = keys_[key].keyState[i - 1];
        }
        keys_[key].keyState[0] = keyState;
        // Determine action type from key state pattern
        uint8_t keyAction = 0;
        if (
           keys_[key].keyState[0] == KEY_WAIT_SHORT
        && keys_[key].keyState[1] == KEY_PRESS_SHORT
        && keys_[key].keyState[2] == KEY_WAIT_SHORT
        && keys_[key].keyState[3] == KEY_PRESS_SHORT
        && keys_[key].keyState[4] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_CLICK_DOUBLE;
        }
        if (
           keys_[key].keyState[0] == KEY_WAIT_LONG
        && keys_[key].keyState[1] == KEY_WAIT_SHORT
        && keys_[key].keyState[2] == KEY_PRESS_SHORT
        && keys_[key].keyState[3] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_CLICK;
        }
        if (
           keys_[key].keyState[0] == KEY_PRESS_LONG
        && keys_[key].keyState[1] == KEY_PRESS_SHORT
        && keys_[key].keyState[2] == KEY_WAIT_SHORT
        && keys_[key].keyState[3] == KEY_PRESS_SHORT
        && keys_[key].keyState[4] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_HOLD_DOUBLE;
        }
        if (
           keys_[key].keyState[0] == KEY_PRESS_LONG
        && keys_[key].keyState[1] == KEY_PRESS_SHORT
        && keys_[key].keyState[2] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_HOLD;
        }
        // Call key handler with key action
        if (keyAction && keyProcesing_) keyProcesing_(key, keyAction);
      }
    }
  }
  return getLastResult();
}
