#include <ArduinoPins.h>
#include <FatReader.h>
#include <FatStructs.h>
#include <mcpDac.h>
#include <SdInfo.h>
#include <SdReader.h>
#include <WaveHC.h>
#include <Wavemainpage.h>
#include <WavePinDefs.h>
#include <WaveUtil.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define COLOR_PIN 3
#define POWER_PIN 5
#define PIN_COUNT 1



// Initialize the "strip" (continuum) of NeoPixels
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIN_COUNT, COLOR_PIN, NEO_RGB + NEO_KHZ400);

void setup()
{
    // Serial connection at speed
    Serial.begin(57600);
    Serial.println("Started setup");
    // Set up the GPIO pins
    pinMode(POWER_PIN, OUTPUT);
    pinMode(COLOR_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
    // Initialize the LEDs
    strip.begin();
    strip.show();
}

void loop()
{
    //Serial.println("Acceleration read start");
    AccelerationReading reading = Bean.getAcceleration();
    char x = abs(reading.xAxis) / 2;
    char y = abs(reading.yAxis) / 2;
    char z = abs(reading.zAxis) / 2;

    if (detectLidIsOpen(x, y, z)) {
        Serial.println("Open!");
        digitalWrite(POWER_PIN, HIGH);
        strip.setBrightness(255);
        strip.show();
        //colorAll(strip.Color(0, 255, 0)); // G
        //colorAll(strip.Color(255, 0, 0)); // R
        //colorAll(strip.Color(0, 0, 255)); // B
        //colorAll(strip.Color(42, 49, 147));
        //colorAll(strip.Color(154, 217, 213));
        int startColor[] = {42,49,147};
        int endColor[] = {154,217,213};
        pulseColors(startColor, endColor, 2000, false);
    } else {
        // https://punchthrough.com/products/bean/reference#LED_setLed
        Bean.setLed(0,0,0);
        digitalWrite(POWER_PIN, LOW);
        strip.setBrightness(0);
        strip.show();
        Serial.println("Not open");
    }
}


bool detectLidIsOpen(char x, char y, char z) {
    /***
     * Open on desk:
     *   X: 189       Y: 8         Z: -167
     *   X: 188       Y: 5         Z: -169
     *   X: 176       Y: 3         Z: -161
     *   X: 180       Y: 4         Z: -173
     * Closed on desk:
     *   X: 4         Y: 5         Z: -244
     *   X: 0         Y: 7         Z: -242
     *   X: 0         Y: 3         Z: -230
     *   X: -6        Y: 7         Z: -245
     *
     * Those raw values are abs'd and halved before being input here
     ***/
    int openThresholdX = 50;
    int openThresholdY = 0;
    int openThresholdZ = 100;

    bool checkOnX = true;
    bool checkOnY = false;
    bool checkOnZ = false;

    bool xSaysOpen = false;
    bool ySaysOpen = false;
    bool zSaysOpen = false;

    if(checkOnX) {
        if (int(x) > openThresholdX) {
            xSaysOpen = true;
        }
    } else {
        xSaysOpen = true;
    }

    if (checkOnY) {
        if (int(y) > openThresholdY) {
            ySaysOpen = true;
        }
    } else {
        ySaysOpen = true;
    }

    if (checkOnZ) {
        // Z-axis is SMALLER when opened, rather than larger
        if (int(z) < openThresholdZ) {
            zSaysOpen = true;
        }
    } else {
        zSaysOpen = true;
    }

    return xSaysOpen && ySaysOpen && zSaysOpen;

}

// Fill the dots one after the other with a color
void colorAll(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
}

// This may or may not be salvageable as a way to light the bean and the NeoPixels at once
void pulseColors(int arrStartColor[], int arrEndColor[], int endToEndTime, bool hasRecursed) {
    Serial.println("Start pulse cycle");
    uint32_t colorStart = strip.Color(arrStartColor[0], arrStartColor[1], arrStartColor[2]);
    uint32_t colorEnd = strip.Color(arrEndColor[0], arrEndColor[1], arrEndColor[2]);
    int binDownTime = int(endToEndTime / 2);
    int binDownDelay = int(binDownTime / 255);
    Serial.println(binDownDelay);
    int currentBrightness = 255;
    bool isDecreasing = true;
    Bean.setLed(arrStartColor[0], arrStartColor[1], arrStartColor[2]);
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
                Bean.setLed(arrEndColor[0], arrEndColor[1], arrEndColor[2]);
            }
            //Serial.println(currentBrightness);
            strip.setBrightness(currentBrightness);
            strip.show();
            delay(binDownDelay);
        }
    }
    Serial.println("Finished pulse cycle");
    if (!hasRecursed) {
        pulseColors(arrEndColor, arrStartColor, endToEndTime, true);
    }
}

// Adafruit color routines
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}



void pulseColorsNP(uint32_t colorStart, uint32_t colorEnd, int endToEndTime) {
    /***
     * This color pulser tested fine on the Trinket, so it was directly dumped here
     ***/
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


// TODO SOUND OUTPUT
