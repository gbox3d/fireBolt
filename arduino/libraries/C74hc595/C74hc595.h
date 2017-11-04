class SPI_74hc595_info {
  private :
  int datapin;
  int datapin_in; 
  int clockpin;
  int latchpin;
  byte *data;
  
  
  
  byte _shiftIn(int myDataPin, int myClockPin) { 
    int i;
    int temp = 0;
    int pinState;
    byte myDataIn = 0;
  
    //pinMode(myClockPin, OUTPUT);
    //pinMode(myDataPin, INPUT);
  //we will be holding the clock pin high 8 times (0,..,7) at the
  //end of each time through the for loop
  
  //at the begining of each loop when we set the clock low, it will
  //be doing the necessary low to high drop to cause the shift
  //register's DataPin to change state based on the value
  //of the next bit in its serial information flow.
  //The register transmits the information about the pins from pin 7 to pin 0
  //so that is why our function counts down
    for (i=7; i>=0; i--)
    {
      digitalWrite(myClockPin, 0);
      delayMicroseconds(0.2);
      temp = digitalRead(myDataPin);
      if (temp) {
        pinState = 1;
        //set the bit to 0 no matter what
        myDataIn = myDataIn | (1 << i);
      }
      else {
        //turn it off -- only necessary for debuging
       //print statement since myDataIn starts as 0
        pinState = 0;
      }
  
      digitalWrite(myClockPin, 1);
  
    }
    return myDataIn;
  }
  
  public:
  SPI_74hc595_info(int _datapin,int _clockpin,int _latchpin,int _inputData_pin,int registerNum) {
    datapin = _datapin;
    clockpin = _clockpin;
    latchpin = _latchpin;
    
    datapin_in = _inputData_pin;
    
    pinMode(datapin_in, INPUT);
    pinMode(datapin, OUTPUT);
    pinMode(clockpin, OUTPUT);  
    pinMode(latchpin, OUTPUT);
    
    data = new byte[registerNum];

    for(int i=0;i<registerNum;i++) {
      data[i] = 0;
    }
    
  }

  void doLatch() {
    digitalWrite(latchpin, HIGH);
    delayMicroseconds(20);
    digitalWrite(latchpin, LOW);

  }

  void shiftWriteData(int _data,int registerIndex) {
    shiftOut(datapin, clockpin, MSBFIRST, _data);
    data[registerIndex] = _data;
  }


  void shiftWritePin(int desiredPin, boolean desiredState)
  {

    int _desiredPin = desiredPin % 8;
    int index = desiredPin / 8;

    bitWrite(data[index],_desiredPin,desiredState);

        
    shiftOut(datapin, clockpin, MSBFIRST, data[index]);
    
    //digitalWrite(latchpin, HIGH);
    //digitalWrite(latchpin, LOW);
  }

  
  byte shiftRead()
  {
  //Pulse the latch pin:
    //set it to 1 to collect parallel data
    //digitalWrite(latchpin,1);
    //set it to 1 to collect parallel data, wait
    //delayMicroseconds(20);
    //set it to 0 to transmit data serially  
    //digitalWrite(latchpin,0);

    byte incoming = _shiftIn(datapin_in, clockpin);
    return incoming;
  }
  
  
};