#include <ShiftDisplay.h>

//https://miguelpynto.github.io/ShiftDisplay/
/*
 
 sck (clock) -> 7
 rck(latch)  -> 6
 dio (data)  -> 5
 */

ShiftDisplay display(COMMON_ANODE, 8);

void setup() {
  Serial.begin(115200);
  /*for (int i = 3; i > 0; i--) {
    display.show(i, 400, ALIGN_CENTER); // store number and show it for 400ms
    display.setDot(1, true); // add dot to stored number
    display.show(400); // show number with dot for 400ms
  }*/
  

  //display.show("hello", 1000);
  //display.show("world", 1000); 
  
}

void loop() {
  //display.show();
  for(int i=0;i<16;i++) {
    Serial.println( String("count :") + i);
    display.show(
      i, 
      200 //딜레이값 (실행이 블럭된다.)
    ); 
  }
  
  
}
