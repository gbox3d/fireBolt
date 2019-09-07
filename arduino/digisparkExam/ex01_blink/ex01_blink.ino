//LED on Model A =>1, B => 0   
const int statusPin = 1;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.

  pinMode(statusPin, OUTPUT); 
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(statusPin, HIGH);   // turn the LED on (HIGH is the voltage level)
//   digitalWrite(statusPin, HIGH);
  delay(1000);               // wait for a second
  digitalWrite(statusPin, LOW);    // turn the LED off by making the voltage LOW
//   digitalWrite(statusPin, LOW); 
  delay(1000);               // wait for a second
}
