/*
 *  Class for an IDX Robot Limit Switches
 */

#include "idx_limit.h"
#include "Arduino.h"

IDXLimit::IDXLimit() {
    Serial.println("IDXLimit");
}


IDXPinLimit::IDXPinLimit(int pin) : IDXLimit()  {
    Serial.println("IDXPinLimit");
  
}


IDXMcpLimit::IDXMcpLimit(int pin, int mcp_address) : IDXLimit() {
    Serial.println("IDXMcpLimit");
}