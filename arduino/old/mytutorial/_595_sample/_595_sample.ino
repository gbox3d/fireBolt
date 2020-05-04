const int clock = 7; //SCK
const int latch = 5; //RCK 
const int data = 6;  //DIO

byte value[] ={ B11111001, // 1
                B10100100, // 2
                B10110000, // 3
                B10011001, // 4
                B10010010, // 5
                B10000010, // 6
                B11111000, // 7
                B10000000, // 8
                B10010000, // 9
                B11000000, // 0
                B11111111};// display nothing

byte digit[] ={ B00000001, // left segment
                B00000010,
                B00000100,
                B00001000,
                B00010000,
                B00100000,
                B01000000,
                B10000000}; // right segment

byte ii = 0;

byte v0, v1,v2,v3,v4,v5,v6,v7;
byte vvalue = value[0];
 
void setup() {
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, OUTPUT);


  
  cli();//stop interrupts
  //set timer0 interrupt at 980Hz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  OCR0A = 255;//(must be <256) --> 16000000 / (prescaler*255) = 980 Hz
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);   //prescaler = 64
  TIMSK0 |= (1 << OCIE0A);  
  sei();//allow interrupts
  
  v0=v1=v2=v3=v4=v5=v6=v7=0;  
  
}
 


ISR(TIMER0_COMPA_vect){   

  /*
  ii++;
  if (ii==8) ii=0;

  if (ii==0) { vvalue = v0; }
  else if (ii==1) { vvalue = v1; }    
  else if (ii==2) { vvalue = v2; }
  else if (ii==3) { vvalue = v3; }
  else if (ii==4) { vvalue = v4; }
  else if (ii==5) { vvalue = v5; }
  else if (ii==6) { vvalue = v6; }
  else if (ii==7) { vvalue = v7; }


  digitalWrite(latch,LOW);
  shiftOut(data,clock,MSBFIRST,B11111111);
  shiftOut(data,clock,MSBFIRST,B11111111);
  digitalWrite(latch,HIGH); 

  digitalWrite(latch,LOW);
  shiftOut(data,clock,MSBFIRST,digit[ii]);
  shiftOut(data,clock,MSBFIRST,value[vvalue]);
  digitalWrite(latch,HIGH);  
  */ 

  digitalWrite(latch,LOW);
  shiftOut(data,clock,MSBFIRST,digit[0]);
  shiftOut(data,clock,MSBFIRST,value[0]);
  digitalWrite(latch,HIGH);  

  digitalWrite(latch,LOW);
  shiftOut(data,clock,MSBFIRST,digit[1]);
  shiftOut(data,clock,MSBFIRST,value[1]);
  digitalWrite(latch,HIGH);  

  
}


long i;

void loop() {

  
  
  //v7 = 1;
  //delay(100);
  
}
