
const int pinIsReady2Fire = 13;
const int pinDoFire = 12;

const int pinStartBtn = 11;
const int pinTrigerInput = 10;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(pinDoFire,OUTPUT);
  pinMode(pinIsReady2Fire,OUTPUT);    
  digitalWrite(pinDoFire,LOW); // control fire
  digitalWrite(pinIsReady2Fire,LOW); //display ready to fire
  
  pinMode(pinStartBtn, INPUT_PULLUP);
  pinMode(pinTrigerInput,INPUT);

  Serial.println("{\"r\":\"version\",\"p1\":\"100\",\"did\":8}");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println( 
    String("start btn : ") + digitalRead(pinStartBtn) + String(",") + pinStartBtn + String("/") +
    String("Trigger btn : ") + digitalRead(pinTrigerInput) + String(",") + pinTrigerInput
    );

}
