

#include <mujs.h>


js_State *J;

void setup() {
  // put your setup code here, to run once:
  
  J = js_newstate(NULL, NULL, JS_STRICT);
  //js_dostring(J, "var a=1;", 1);

}

void loop() {
  // put your main code here, to run repeatedly:

}
