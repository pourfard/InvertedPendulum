
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

void setup() {
  pinMode(9,OUTPUT); // set Pin9 as PUL
  pinMode(8,OUTPUT); // set Pin8 as DIR
  Serial.begin(9600);

  // encoder
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void doStep(bool right, int steps, int sleepMicroSeconds)
{
    if(right)
    {
        digitalWrite(8,HIGH); // set high level direction
    }
    else
    {
        digitalWrite(8,LOW); // set high level direction
    }

    for(int x = 0; x < steps; x++) // repeat 400 times a revolution when setting 400 on driver
    {
        digitalWrite(9,HIGH); // Output high
        delayMicroseconds(sleepMicroSeconds); // set rotate speed

        digitalWrite(9,LOW); // Output low
        delayMicroseconds(sleepMicroSeconds); // set rotate speed
    }
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time

  //Serial.write(encoderValue);
}

void loop() {
    //doStep(true, 6400, 50);
    //delay(1000);
    //doStep(false, 6400, 50);
    //delay(1000);

    if (Serial.available() > 0) {
      String incomingString = Serial.readStringUntil('\r');
      String dir = getValue(incomingString, ',', 0);
      String steps = getValue(incomingString, ',', 1);
      String sleepMicroSeconds = getValue(incomingString, ',', 2);
      doStep(dir == "right", steps.toInt(), sleepMicroSeconds.toInt());
  }

    Serial.println("#"+String(encoderValue)+"#");
    delay(1);
}
