#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

const byte LED_CLOCK_PIN  = 7;
const byte LED_DATA_PIN   = 6;
const byte LED_LENGTH = 32;

const byte OPERATION_SWITCH_MODE  = 0;
const byte OPERATION_SET_COLOR    = 1;

const byte MODE_SOLID   = 0;
const byte MODE_RAINBOW = 1;

// Colors are BGR
const uint32_t colorTable[4] = { 0x008FFF, 0xAA0000, 0x00FF00, 0x0000FF };

byte mode = MODE_SOLID;
uint32_t curColor = 0x008FFF;

Adafruit_WS2801 strip = Adafruit_WS2801(LED_LENGTH, LED_DATA_PIN, LED_CLOCK_PIN);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin();
  
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
}

void loop() {

  // send data only when you receive data:
  if (Serial.available() >= 2) {
    // read the incoming operation and arg bytes:
    byte op = Serial.read();
    byte arg = Serial.read();

    Serial.print("Recieved ");
    Serial.print(op);
    Serial.print(", ");
    Serial.println(arg);

    if (op == OPERATION_SWITCH_MODE) {
      mode = arg;
    } else if (op == OPERATION_SET_COLOR) {
      Serial.print("Setting color to ");
      Serial.println(colorTable[arg], HEX);
      curColor = colorTable[arg];
    } else {
      Serial.print("Unknown operation: ");
      Serial.println(op);
    }

    Serial.flush();
  }

  if(mode == MODE_RAINBOW) {
    rainbowCycleTick();
    delay(5);
  } else if(mode == MODE_SOLID) {
    setColor(curColor);
  }
}

// From library sample code
void rainbowCycleTick() {
  int i;
  static int rainbowInc;

    for (i = 0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + rainbowInc) % 256) );
    }
    strip.show();   // write all the pixels out

    rainbowInc++;
}

// From library sample code
//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// From library sample code
// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

// From library sample code
// fill the dots one after the other with said color
// good for testing purposes
void setColor(uint32_t c) {
  // Set all of the pixels to the specified value
  for (int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
}