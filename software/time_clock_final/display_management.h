/*
 * Adafruit 4-Digit 7-Segment LED Matrix Display FeatherWing
 * https://www.adafruit.com/product/3088
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment led7 = Adafruit_7segment();

void begin_led7() {
  led7.begin(0x70);
  for (int i = 0 ; i < 5 ; i++) {
    led7.writeDigitNum(i, 0);
  }
  led7.writeDisplay();
}

