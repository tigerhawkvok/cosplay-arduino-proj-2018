#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 4



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_RGB + NEO_KHZ400);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Some example procedures showing how to display to the pixels:
//   strip.Color(42, 49, 147);
//   strip.setBrightness(255);
//   strip.show();
  int startColor[] = {42,49,147};
        int endColor[] = {104,255,153};
        //pulseColors(startColor, endColor, 2000, false);
        pulseColorsNP(strip.Color(42, 49, 147), strip.Color(104,200,107), 2000, false);
        //rainbow(500); // works
}


void pulseColors(int arrStartColor[], int arrEndColor[], int endToEndTime, bool hasRecursed) {
    //Serial.println("Start pulse cycle");
    uint32_t colorStart = strip.Color(arrStartColor[0], arrStartColor[1], arrStartColor[2]);
    uint32_t colorEnd = strip.Color(arrEndColor[0], arrEndColor[1], arrEndColor[2]);
    int binDownTime = int(endToEndTime / 2);
    int binDownDelay = int(binDownTime / 255);
    //Serial.println(binDownDelay);
    int currentBrightness = 255;
    bool isDecreasing = true;
    //Bean.setLed(arrStartColor[0], arrStartColor[1], arrStartColor[2]);
    for (uint16_t i=0; i < int(endToEndTime/binDownDelay); i++) {
        if (currentBrightness > 0 && isDecreasing) {
            // We're still decreasing
            currentBrightness--;
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            strip.show();
            delay(binDownDelay);
        } else {
            // We're increasing to the next color
            currentBrightness++;
            if (currentBrightness > 255) {
                break;
            }
            if (isDecreasing) {
                isDecreasing = false;
                for(uint16_t i2=0; i2<strip.numPixels(); i2++) {
                    strip.setPixelColor(i2, colorEnd);
                }
                //Bean.setLed(arrEndColor[0], arrEndColor[1], arrEndColor[2]);
            }
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            strip.show();
            delay(binDownDelay);
        }
    }
    //Serial.println("Finished pulse cycle");
    if (!hasRecursed) {
        pulseColors(arrEndColor, arrStartColor, endToEndTime, true);
    }
}


void pulseColorsNP(uint32_t colorStart, uint32_t colorEnd, int endToEndTime, bool hasRecursed) {
    //Serial.println("Start pulse cycle");
    int binDownTime = int(endToEndTime / 2);
    int binDownDelay = int(binDownTime / 255);
    //Serial.println(binDownDelay);
    int currentBrightness = 255;
    bool isDecreasing = true;
    delay(binDownDelay);
    //Bean.setLed(arrStartColor[0], arrStartColor[1], arrStartColor[2]);
    for (uint16_t i=0; i < 512; i++) {
        if (currentBrightness > 0 && isDecreasing) {
            // We're still decreasing
            currentBrightness--;
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            for(uint16_t i2=0; i2<strip.numPixels(); i2++) {
                strip.setPixelColor(i2, colorStart);
            }
            strip.show();
            delay(binDownDelay * 2);
        }
        if (currentBrightness <= 1 || !isDecreasing) {
            // We're increasing to the next color
            currentBrightness++;
            if (currentBrightness > 255) {
                break;
            }
            if (isDecreasing) {
                isDecreasing = false;
                //Bean.setLed(arrEndColor[0], arrEndColor[1], arrEndColor[2]);
            }
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            for(uint16_t i2=0; i2<strip.numPixels(); i2++) {
                strip.setPixelColor(i2, colorEnd);
            }
            strip.show();
            delay(binDownDelay);
        }
    }
    currentBrightness = 255;
    isDecreasing = true;
    for (uint16_t i=0; i < 512; i++) {
        if (currentBrightness > 0 && isDecreasing) {
            // We're still decreasing
            currentBrightness--;
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            for(uint16_t i2=0; i2<strip.numPixels(); i2++) {
                strip.setPixelColor(i2, colorEnd);
            }
            strip.show();
            delay(binDownDelay * 2);
        }
        if (currentBrightness <= 1 || !isDecreasing) {
            // We're increasing to the next color
            currentBrightness++;
            if (currentBrightness > 255) {
                break;
            }
            if (isDecreasing) {
                isDecreasing = false;
                //Bean.setLed(arrEndColor[0], arrEndColor[1], arrEndColor[2]);
            }
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            for(uint16_t i2=0; i2<strip.numPixels(); i2++) {
                strip.setPixelColor(i2, colorStart);
            }
            strip.show();
            delay(binDownDelay);
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
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
