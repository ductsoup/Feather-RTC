/*
 * Manage RTC Daylight Saving Time
 * 
 * DS3231 Precision RTC FeatherWing - RTC Add-on For Feather Boards
 * https://www.adafruit.com/product/3028
 */
#include <SPI.h>
#include <RTClib.h>   // https://github.com/adafruit/RTClib

RTC_DS3231 rtc = RTC_DS3231();

#define TIME_ZONE -5  // America/New York

void begin_rtc() {
  rtc.begin();
}

/*
 * For debugging.
 */
void showDate(const char* txt, const DateTime& dt) {
  char buf[128];
  sprintf(buf, "%d = %04d/%02d/%02d %02d:%02d:%02d %s", dt.unixtime(), dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), txt);
  Serial.println(buf);
}

void showTimeSpan(const char* txt, const TimeSpan& ts) {
  char buf[128];
  sprintf(buf, "%d days %d hours %d minutes %d seconds %d total seconds %s", ts.days(), ts.hours(), ts.minutes(), ts.seconds(), ts.totalseconds(), txt);
  Serial.println(buf);
}

/*
 * Determine whether or not it is daylight saving time for a given time zone given a
 * NTS timestamp from WiFi101. 
 * 
 * For the sake of sanity convert GMT to EST (or wuteveh), then figure out DST from 
 * there. We don't need to worry about the year difference around 12/31 and 1/1 because 
 * both months are in EST by definition.
 * 
 * Personal option ... daylight saving time was one of the dumbest ideas ever.
 * 
 * References:
 * 
 * https://github.com/adafruit/RTClib/blob/master/examples/datecalc/datecalc.ino
 * https://github.com/adafruit/RTClib/blob/master/RTClib.h
 *
 * Starting in 2007, DST begins in the United States on the second Sunday in March,
 * when people move their clocks forward an hour at 2 a.m. local standard time
 * (so at 2 a.m. on that day, the clocks will then read 3 a.m. local daylight time).
 * Daylight saving time ends on the first Sunday in November, when clocks are moved
 * back an hour at 2 a.m. local daylight time (so they will then read 1 a.m. local
 * standard time).
 *
 * Last year, DST began on March 12 and ended on Nov. 5. And this year, DST began
 * on March 11 and ends on Nov. 4, 2018. You will then move your clock forward an
 * hour on March 10, 2019, and the cycle will begin again.
 *
 * If the 1st is Saturday, the first weekday is on DATE(<year>, <month>, 3)
 * If the 1st is Sunday, the first weekday is on DATE(<year>, <month>, 2)
 * Otherwise the 1st is a weekday, so just use DATE(<year>, <month>, 1)
 *
 * In the U.S., clocks change at 2:00 a.m. local time.
 * In spring, clocks spring forward from 1:59 a.m. to 3:00 a.m.
 * In fall, clocks fall back from 1:59 a.m. to 1:00 a.m.
 */
bool isDST(unsigned long gmt_epoch) {
  DateTime gmt (gmt_epoch);
  DateTime std (gmt + TimeSpan(0, TIME_ZONE, 0, 0));
  int day_offset;
  bool ret;
  while (true) {
    if (std.month() < 3 || std.month() > 11) {          // Clearly standard time
      ret = false;
      break;
    } else if (std.month() > 3 && std.month() < 11) {   // Clearly daylight saving time
      ret = true;
      break;
    } else if (std.month() == 3) {                      // March
      DateTime buf (std.year(), 3, 1, 1, 59, 59);       // Find the second Sunday
      buf = buf + TimeSpan(14 - buf.dayOfTheWeek(), 0, 0, 0);
      ret = (std.unixtime() > buf.unixtime()) ? true : false;
      break;
    } else {                                            // November
      DateTime buf (std.year(), 11, 1, 0, 59, 59);      // Find the first Sunday
      buf = buf + TimeSpan(7 - buf.dayOfTheWeek(), 0, 0, 0);
      ret = (std.unixtime() > buf.unixtime()) ? false : true;
      break;
    }
  }
  return ret;
}

/*
 *  Update the RTC considering daylight savings. 
 *  
 *  Typically you'd call this function from loop() daily at 2am. If you're bold you 
 *  really only have to call it in the first week or two of March and November. The
 *  RTC should do the rest.
 */
void update_rtc() {
  unsigned long epoch = 0;

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.end();
    delay(1000);
    begin_wifi();
  }
  if (WiFi.status() == WL_CONNECTED) {
    epoch = WiFi.getTime();
  }
  if (epoch) {
    DateTime gmt (epoch);
    DateTime local (gmt + TimeSpan(0, isDST(epoch) ? (TIME_ZONE + 1) : TIME_ZONE, 0, 0));
    rtc.adjust(local.unixtime());
    showDate("Sync", local);
  }
}

/*
 * As always, trust but verify.
 */
void test_dst() {
  DateTime buf3 (2018, 3, 11, 1, 59, 59);
  showDate(isDST(buf3.unixtime() + 5 * 60 * 60) ? "DAYLIGHT" : "STANDARD", buf3);
  buf3 = buf3 + TimeSpan(0, 0, 0, 1);
  showDate(isDST(buf3.unixtime() + 5 * 60 * 60) ? "DAYLIGHT" : "STANDARD", buf3);
  buf3 = buf3 + TimeSpan(0, 0, 0, 1);
  showDate(isDST(buf3.unixtime() + 5 * 60 * 60) ? "DAYLIGHT" : "STANDARD", buf3);
  Serial.println("-----");

  DateTime buf11 (2018, 11, 4, 1, 59, 59);
  showDate(isDST(buf11.unixtime() + 4 * 60 * 60) ? "DAYLIGHT" : "STANDARD", buf11);
  buf11 = buf11 + TimeSpan(0, 0, 0, 1);
  showDate(isDST(buf11.unixtime() + 4 * 60 * 60) ? "DAYLIGHT" : "STANDARD", buf11);
  buf11 = buf11 + TimeSpan(0, 0, 0, 1);
  showDate(isDST(buf11.unixtime() + 4 * 60 * 60) ? "DAYLIGHT" : "STANDARD", buf11);
  Serial.println("=====");
}

