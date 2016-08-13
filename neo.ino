#include "SPI.h"
#include "Adafruit_WS2801.h"

/*****************************************************************************
Example sketch for driving Adafruit WS2801 pixels!


  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!

uint8_t dataPin  = 10;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 9;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(50, dataPin, clockPin);

// Here is where you can put in your favorite colors that will appear!
// just add new {nnn, nnn, nnn}, lines. They will be picked out randomly
//                          R   G   B
uint8_t myColors[][3] = {{232, 100, 255},   // purple
                         {200, 200, 20},   // yellow
                         {30, 200, 200},   // blue
                          };

// managing button state and mode
#define BUTTON_PIN   6
#define NUM_MODES    5 // how many modes there are
bool oldState = HIGH;
uint8_t mode = 0;


// don't edit the line below
#define FAVCOLORS sizeof(myColors) / 3

void setup() {
  Serial.begin(19200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// Checks the button state and returns whether the button has been
// pushed or not.
bool checkButton() {
  bool newState = digitalRead(BUTTON_PIN);
  bool has_changed = newState != oldState;
  if (has_changed) {
      // change the LED mode
      mode = (mode + 1) % NUM_MODES;
  }
  oldState = newState;
  return has_changed;
}
uint8_t num_iterations = 0;
void loop() {
    num_iterations++;
    Serial.print(num_iterations, DEC);
    Serial.println(" iterations");

    checkButton();
    switch(mode) {
    case 0:
        Serial.println("mode 0");
        flashRandom(5, 10);
        break;
    case 1:
        Serial.println("rainbow cycle 10");
        rainbowCycle(10);
        break;
    case 2:
        Serial.println("flash random 5");
        flashRandom(5, 5);
        break;
    case 3:
        Serial.println("rainbow cycle");
        rainbowCycle(20);
        break;
    case 4:
        Serial.println("flash random 12");
        flashRandom(5, 20);
        break;
    }
    clearPixels();
}

// Reset everything to black (but does not "show" them)
void clearPixels() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Color(0,0,0));
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);

      if(checkButton()) {
          return;
      }
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);

    if(checkButton()) {
      return;
    }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);

    // Because this function takes a long time to run, you should call
    // checkButton() once and a while.  If if returns true, you should
    // exit out of the rainbowCycle function because it means the
    // button has been pushed and we want to switch modes!
    if(checkButton()) {
      return;
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void flashRandom(int wait, uint8_t howmany) {

  for(uint16_t i=0; i<howmany; i++) {
    // pick a random favorite color!
    int c = random(FAVCOLORS);
    int red = myColors[c][0];
    int green = myColors[c][1];
    int blue = myColors[c][2];

    // get a random pixel from the list
    int j = random(strip.numPixels());

    // now we will 'fade' it in 5 steps
    for (int x=0; x < 5; x++) {
      int r = red * (x+1); r /= 5;
      int g = green * (x+1); g /= 5;
      int b = blue * (x+1); b /= 5;

      strip.setPixelColor(j, Color(r, g, b));
      strip.show();
      delay(wait);
    }
    // & fade out in 5 steps
    for (int x=5; x >= 0; x--) {
      int r = red * x; r /= 5;
      int g = green * x; g /= 5;
      int b = blue * x; b /= 5;

      strip.setPixelColor(j, Color(r, g, b));
      strip.show();
      delay(wait);
    }
    if(checkButton()) {
      return;
    }
  }
  // LEDs will be off when done (they are faded to 0)
}

/* Helper functions */

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
