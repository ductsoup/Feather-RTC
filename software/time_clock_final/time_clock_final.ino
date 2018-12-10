/*
   Adafruit Feather M0 WiFi - ATSAMD21 + ATWINC1500
   https://www.adafruit.com/product/3010

   Adafruit 4-Digit 7-Segment LED Matrix Display FeatherWing
   https://www.adafruit.com/product/3088

   DS3231 Precision RTC FeatherWing - RTC Add-on For Feather Boards
   https://www.adafruit.com/product/3028
   https://www.adafruit.com/product/2830

    Adafruit Power Relay FeatherWing
    https://www.adafruit.com/product/3191
*/

#define DEBUG true

#include "relay_management.h"
#include "wifi_management.h"
#include "display_management.h"
#include "rtc_management.h"

void setup() {
  // Initialize the serial debug monitor
  Serial.begin(115200);
  delay(3000);

  // Initialize the 7-segment LED display
  begin_led7();

  // Initialize the relay
  begin_relay();
  
  // Initialize WiFi
  begin_wifi();

  // Initialize RTC
  begin_rtc();
  update_rtc();
#ifdef DEBUG
  test_dst();
#endif
}

// Horn schedule (24hour, minute)
#define PULSE_LENGTH 2
const unsigned int pulse[][2] = {
  {  8,  0 },   //  8:00 am
  { 12,  0 },   // 12:00 pm
  { 12, 30 },   // 12:30 pm
  { 16, 30 }    // 04:30 pm
};

bool colon = false;

void loop() {
  DateTime now = rtc.now();
  int i;

  // Update the LED display
  // The first decimal is the WiFi connection status, the rest are 15, 30 and 45 second intervals
  led7.writeDigitNum(0, now.hour() < 10 ? 0 : int(now.hour() / 10), WiFi.status() == WL_CONNECTED);
  led7.writeDigitNum(1, now.hour() % 10, now.second() < 15 ? false : true);
  led7.writeDigitNum(3, now.minute() < 10 ? 0 : int(now.minute() / 10), now.second() < 30 ? false : true);
  led7.writeDigitNum(4, now.minute() % 10, now.second() < 45 ? false : true);
  led7.drawColon(colon = !colon);
  led7.writeDisplay();

  // Update the horn relay
  for (i = 0 ; i < (sizeof(pulse) / (2 * sizeof(unsigned int))) ; i++) {
    if (now.hour() == pulse[i][0] && now.minute() == pulse[i][1]) {
      if (now.second() == 0 && getRelay() == LOW) {
        setRelay(HIGH);
        showDate("Horn on", now);
      }
      if (now.second() == PULSE_LENGTH && getRelay() == HIGH) {
        setRelay(LOW);
        showDate("Horn off", now);
      }
      break;
    }
  }

  // Adust the RTC daily at 02:00:00
  if (now.hour() == 2 && now.minute() == 0 && now.second() == 30) {
  //if (now.minute() % 5 == 0 && now.second() == 30) {
    update_rtc();
  }
  delay(1000);
}
