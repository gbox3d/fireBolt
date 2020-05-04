void setup() {
    Serial.begin(9600);               //initial the Serial
    Serial1.begin(9600);
}

void loop()
{
    if(Serial.available())
    {
        Serial1.write(Serial.read());    //send what has been received
    }
    
    if(Serial1.available())
    {
        Serial.write(Serial1.read());    //send what has been received
    }
    
}
