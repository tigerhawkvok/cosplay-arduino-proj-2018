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
        //Bean.setLed(42, 49, 147);
        Bean.setLed(154, 217, 213);
        // String coords = String(x) + ", " + String(y) + ", " + String(z);
        // Serial.println(coords);
        Serial.println("Open!");
        digitalWrite(POWER_PIN, HIGH);
        strip.setBrightness(255);
        strip.show();
        colorAll(strip.Color(0, 255, 0)); // G
        //colorAll(strip.Color(255, 0, 0)); // R
        //colorAll(strip.Color(0, 0, 255)); // B
        //colorAll(strip.Color(42, 49, 147));
        //colorAll(strip.Color(154, 217, 213));
        pulseColors(strip.Color(42, 49, 147), strip.Color(154, 217, 213), 2000, false);
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
     ***/
    int openThresholdX = 50;
    int openThresholdY = 0;
    int openThresholdZ = 0;

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
        if (int(z) > openThresholdZ) {
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


void pulseColors(uint32_t colorStart, uint32_t colorEnd, int endToEndTime, bool hasRecursed) {
    Serial.println("Start pulse cycle");
    int binDownTime = int(endToEndTime / 2);
    int binDownDelay = int(binDownTime / 255);
    int currentBrightness = 255;
    bool isDecreasing = true;
    for (uint16_t i=0; i < binDownDelay; i++) {
        if (currentBrightness > 0 && isDecreasing) {
            // We're still decreasing
            currentBrightness--;
            strip.setBrightness(currentBrightness);
            strip.show();
            delay(binDownDelay);
        } else {
            // We're increasing to the next color
            isDecreasing = false;
            currentBrightness++;
            for(uint16_t j=0; i<strip.numPixels(); j++) {
                strip.setPixelColor(j, colorEnd);
            }
            strip.setBrightness(currentBrightness);
            strip.show();
            delay(binDownDelay);
        }
    }
    Serial.println("Finished pulse cycle");
    if (!hasRecursed) {
        pulseColors(colorEnd, colorStart, endToEndTime, true);
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
