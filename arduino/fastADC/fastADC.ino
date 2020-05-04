//
//#define MAXBUFFER 30000
//
//byte g_buffer[MAXBUFFER];
//
//int g_bufIndex=0;
long t, t0;

void setup()
{
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
}


byte adcValue = 0;
ISR(ADC_vect)
{
  adcValue = ADCH;  // read 8 bit value from ADC
//  
//  g_buffer[g_bufIndex] = adcValue;
//  
//  g_bufIndex++;
//  g_bufIndex %= MAXBUFFER;
  
  //numSamples++;
}
  
void loop()
{

  if( micros() - t0 > 1000000)
  {
    Serial.println(adcValue);
    t0 = micros();
  }

  
  
  
//  if (numSamples>=1000)
//  {
//    t = micros()-t0;  // calculate elapsed time
//
//    Serial.print("Sampling frequency: ");
//    Serial.print((float)1000000/t);
//    Serial.println(" KHz");
//    delay(2000);
//    
//    // restart
//    t0 = micros();
//    numSamples=0;
//  }
}
