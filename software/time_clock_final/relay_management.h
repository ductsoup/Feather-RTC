#define RELAY_PIN 11

void begin_relay() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void setRelay(int state) {
  digitalWrite(LED_BUILTIN, state);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(RELAY_PIN, state);
}

int getRelay() {
  return digitalRead(RELAY_PIN);
}
