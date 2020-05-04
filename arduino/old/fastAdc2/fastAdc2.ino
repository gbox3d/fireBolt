void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
  Serial.println("start");
}

//byte adcValue = 0;
//byte fsm = 0;
//

//#define MAXBUFFER 128
//byte g_buffer[MAXBUFFER];
//int g_bufIndex=0;

int fsm = 0;
int count=0;
int _levelTh = 0;

byte val01=0,val02=0;

unsigned long _start_tick = 0;

ISR(ADC_vect)
{
  val01 =  ADCH;  // read 8 bit value from ADC

  volatile int diff = val02 - val01;

//  Serial.println(diff);
  
  switch(fsm) {
    case 0:
    {
      if(diff > 0) {
        fsm = 10; //high
        if(count >2 )
          _start_tick = micros();
         
      }
      else if(diff < 0)
      {
        fsm = 20; // low
        if(count > 2)
          _start_tick = micros();
      }
      break;
    }
    case 10:
    {
      if(diff < 0) {
        fsm = 0;
        if(_levelTh > 5) {
          count++;
          _levelTh = 0;
        }
        
      }
      else if(diff > 0) {
        _levelTh++;
      }
    }
    break;
    case 20:
    {
      if(diff > 0) {
        fsm = 0;
        if(_levelTh > 5) {
          count++;
          _levelTh = 0;
        }
      }
      else if(diff < 0) {
        _levelTh++;
      }
    }
    break;
  }
  val02 = val01;

  
  
  
//  if(adcValue > (131+2) || adcValue < (131-2)) {
//    if( g_bufIndex < MAXBUFFER) {
//      g_buffer[g_bufIndex] = adcValue;
//      g_bufIndex++;
//    }
//  }
  
}

void loop() {

  if(count > 32) {
//    Serial.println(count);
noInterrupts();
    unsigned long  duty = micros() - _start_tick;
    Serial.println(duty);
    
    count = 0;
    interrupts();
    
  }
  
  // put your main code here, to run repeatedly:
//  if(g_bufIndex > 64) {
//    
//    for(int i=0;i<64;i++) {
//      Serial.print(g_buffer[i]);
//      Serial.print(",");
//    }
//    Serial.println("----");
//    g_bufIndex = 0;
//  }

}
