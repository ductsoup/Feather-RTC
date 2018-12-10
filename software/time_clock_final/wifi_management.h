/*
 * Adafruit Feather M0 WiFi - ATSAMD21 + ATWINC1500
 * https://www.adafruit.com/product/3010
 */
#include <WiFi101.h>

#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2

char *access_points[][2] = {
  {"myssid1", "mypassword1"},
  {"myssid2", "mypassword2"}
};

/*
 * Connect or reconnect to an access point
 */
void begin_wifi() {
  int i = -1;
  int status = WL_IDLE_STATUS;

  WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);
#ifdef DEBUG
  Serial.print("WiFi Status: ");
  Serial.println(status);
#endif
  if (WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG
    Serial.println("WiFi shield not present");
#endif
  } else {
    while ( status != WL_CONNECTED) {
      ++i = i >= sizeof(access_points) / (2 * sizeof(char*)) ? 0 : i;
#ifdef DEBUG
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(access_points[i][0]);
#endif
      status = WiFi.begin(access_points[i][0], access_points[i][1]);
      delay(10000);
    }

#ifdef DEBUG
    Serial.print("WiFi Status: ");
    Serial.println(status);
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);
    IPAddress ip = WiFi.localIP();
    Serial.print("IP:  ");
    Serial.println(ip);
#endif
  }
}

