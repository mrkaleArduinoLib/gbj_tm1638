#include "gbj_tm1638.h"
const String gbj_tm1638::VERSION = "GBJ_TM1638 1.0.0";


gbj_tm1638::gbj_tm1638(uint8_t pinClk, uint8_t pinDio, uint8_t pinStb, \
  uint8_t digits, uint8_t leds, uint8_t keys)
{
  _status.pinClk = pinClk;
  _status.pinDio = pinDio;
  _status.pinStb = pinStb;
  _status.digits = min(digits, getDigitsMax());
  _status.leds = min(leds, getLedsMax());
  _status.keys = min(keys, getKeysMaxHw());
}


uint8_t gbj_tm1638::begin()
{
  initLastResult();
  // Check pin duplicity
  if (_status.pinClk == _status.pinDio \
  ||  _status.pinDio == _status.pinStb \
  ||  _status.pinStb == _status.pinClk) return setLastResult(ERROR_PINS);
  // Setup pins
  pinMode(_status.pinClk, OUTPUT);
  pinMode(_status.pinDio, OUTPUT);
  pinMode(_status.pinStb, OUTPUT);
  // Initialize controller
  return setContrast();
}


//------------------------------------------------------------------------------
// Software manipulation - updating screen buffer
//------------------------------------------------------------------------------
// Print one character determined by a byte of ASCII code
size_t gbj_tm1638::write(uint8_t ascii)
{
  if (_print.digit >= _status.digits) return 0;
  uint8_t mask = getFontMask(ascii);
  if (mask == FONT_MASK_WRONG)
  {
    if (String(".,:").indexOf(ascii) >= 0)  // Detect radix
    {
      printRadixOn(_print.digit - 1); // Set radix to the previous digit
    }
    return 0;
  }
  else
  {
    printDigit(_print.digit, mask);
    return 1;
  }
}


// Print null terminated character array
size_t  gbj_tm1638::write(const char* text)
{
  uint8_t digits = 0;
  uint8_t i = 0;
  while (text[i] != '\0' && _print.digit < _status.digits)
  {
    digits += write(text[i++]);
  }
  return digits;
}


// Print byte array with length
size_t  gbj_tm1638::write(const uint8_t* buffer, size_t size)
{
  uint8_t digits = 0;
  for (uint8_t i = 0; i < size && _print.digit < _status.digits; i++)
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
  if (busSend(CMD_ADDR_INIT, _print.buffer, bufferLen)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_tm1638::displayOn()
{
  return setContrast(_status.contrast);
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
  _keyProcesing = handler;
}


void gbj_tm1638::run()
{
  if (_status.keys == 0) return; // No key processing when no key is enabled
  uint32_t tsNow = millis();
  if (tsNow - _status.scanTimestamp >= TIMING_SCAN)
  {
    _status.scanTimestamp = tsNow;
    processKeypad();
  }
}

//------------------------------------------------------------------------------
// Setters
//------------------------------------------------------------------------------
uint8_t gbj_tm1638::setContrast(uint8_t contrast)
{
  _status.contrast = contrast & getContrastMax();
  return busSend(CMD_DISP_INIT | CMD_DISP_ON | _status.contrast);
}


void gbj_tm1638::setFont(const uint8_t* fontTable, uint8_t fontTableSize)
{
  _font.table = fontTable;
  _font.glyphs = fontTableSize / FONT_WIDTH;
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
  digitalWrite(_status.pinStb, HIGH); // Finish previous communication for sure
  digitalWrite(_status.pinClk, HIGH);
  digitalWrite(_status.pinStb, LOW); // Start communication
}


// Stop condition - pull up STB from LOW to HIGH
void gbj_tm1638::endTransmission()
{
  digitalWrite(_status.pinStb, HIGH);
}


void gbj_tm1638::busWrite(uint8_t data)
{
  digitalWrite(_status.pinClk, LOW); // For active rising edge of clock pulse
  shiftOut(_status.pinDio, _status.pinClk, LSBFIRST, data);
}


uint8_t gbj_tm1638::busRead()
{
  digitalWrite(_status.pinClk, LOW); // For active rising edge of clock pulse
  uint8_t data = shiftIn(_status.pinDio, _status.pinClk, LSBFIRST);
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
  pinMode(_status.pinDio, INPUT);
  for (uint8_t bufferIndex = 0; bufferIndex < BYTES_SCAN; bufferIndex++)
  {
    buffer[bufferIndex] = busRead();
  }
  pinMode(_status.pinDio, OUTPUT);
  endTransmission();
  return getLastResult();
}


// The method leaves digit cursor after last print digit
void gbj_tm1638::gridWrite(uint8_t segmentMask, uint8_t gridStart, uint8_t gridStop)
{
  swapByte(gridStart, gridStop);
  gridStop = min(gridStop, _status.digits - 1);
  for (_print.digit = gridStart; _print.digit <= gridStop; _print.digit++)
  {
    segmentMask &= 0x7F; // Clear radix bit in segment mask
    _print.buffer[addrGrid(_print.digit)] &= 0x80;  // Clear digit bits in screen buffer
    _print.buffer[addrGrid(_print.digit)] |= segmentMask;  // Set digit bits but leave radix bit intact
  }
}


uint8_t gbj_tm1638::getFontMask(uint8_t ascii)
{
  uint8_t mask = FONT_MASK_WRONG;
  for (uint8_t glyph = 0; glyph < _font.glyphs; glyph++)
  {
    if (ascii == pgm_read_byte(&_font.table[glyph * FONT_WIDTH + FONT_INDEX_ASCII]))
    {
      mask = pgm_read_byte(&_font.table[glyph*2 + FONT_INDEX_MASK]);
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
  for (uint8_t bus = 0; bus < _status.keys / 8 + 1; bus++)
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
      if (key >= _status.keys) break;
      bool keyPressed = keyMask & (1 << keyBit);
      uint8_t keyState;
      // Determine current key state
      if (keyPressed)
      {
        _keys[key].waitScans = 0;
        keyState = KEY_PRESS_SHORT;
        if (_keys[key].pressScans < 255) _keys[key].pressScans++;
        if (_keys[key].pressScans >= TIMING_SCAN_TRESHOLD_PRESS_LONG) keyState = KEY_PRESS_LONG;
      }
      else
      {
        _keys[key].pressScans = 0;
        keyState = KEY_WAIT_SHORT;
        if (_keys[key].waitScans < 255) _keys[key].waitScans++;
        if (_keys[key].waitScans >= TIMING_SCAN_TRESHOLD_WAIT) keyState = KEY_WAIT_LONG;
      }
      // Process action if state has changed
      if (_keys[key].keyState[0] != keyState)
      {
        // Historize key states
        for (uint8_t i = sizeof(_keys[key].keyState) / sizeof(_keys[key].keyState[0]) - 1; i > 0; i--)
        {
          _keys[key].keyState[i] = _keys[key].keyState[i - 1];
        }
        _keys[key].keyState[0] = keyState;
        // Determine action type from key state pattern
        uint8_t keyAction = 0;
        if (
           _keys[key].keyState[0] == KEY_WAIT_SHORT
        && _keys[key].keyState[1] == KEY_PRESS_SHORT
        && _keys[key].keyState[2] == KEY_WAIT_SHORT
        && _keys[key].keyState[3] == KEY_PRESS_SHORT
        && _keys[key].keyState[4] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_CLICK_DOUBLE;
        }
        if (
           _keys[key].keyState[0] == KEY_WAIT_LONG
        && _keys[key].keyState[1] == KEY_WAIT_SHORT
        && _keys[key].keyState[2] == KEY_PRESS_SHORT
        && _keys[key].keyState[3] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_CLICK;
        }
        if (
           _keys[key].keyState[0] == KEY_PRESS_LONG
        && _keys[key].keyState[1] == KEY_PRESS_SHORT
        && _keys[key].keyState[2] == KEY_WAIT_SHORT
        && _keys[key].keyState[3] == KEY_PRESS_SHORT
        && _keys[key].keyState[4] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_HOLD_DOUBLE;
        }
        if (
           _keys[key].keyState[0] == KEY_PRESS_LONG
        && _keys[key].keyState[1] == KEY_PRESS_SHORT
        && _keys[key].keyState[2] == KEY_WAIT_LONG
        )
        {
          keyAction = KEY_HOLD;
        }
        // Call key handler with key action
        if (keyAction && _keyProcesing) _keyProcesing(key, keyAction);
      }
    }
  }
  return getLastResult();
}
