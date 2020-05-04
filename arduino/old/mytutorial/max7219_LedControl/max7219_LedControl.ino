#include "LedControl.h"

LedControl lc=LedControl(12,11,10,2);  // Pins: DIN,CLK,CS, # of Display connected

void setup()
{
  lc.shutdown(0,false);  // Wake up displays
  lc.setIntensity(0,5);  // Set intensity levels
  lc.clearDisplay(0);  // Clear Displays
}


void loop()
{
  lc.setRow(0,0,B10000000);
  delay(500);
  lc.setRow(0,0,B01000000);
  delay(500);
}

