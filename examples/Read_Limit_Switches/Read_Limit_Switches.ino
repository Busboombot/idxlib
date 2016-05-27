#include <Wire.h>
#include <Adafruit_MCP23008.h>
#include <idx_limit.h>

IDXMcpLimit limits[] = {IDXMcpLimit(0), IDXMcpLimit(1), IDXMcpLimit(2)};
#define NUM_LIMITS 3

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_LIMITS; i++) {
    limits[i].begin();
  }
}

void loop() {
  for (int i = 0; i < NUM_LIMITS; i++) {
    Serial.print(limits[i].isInLimit());
  }
  Serial.println("");
}
