int x;
void setup() {
  pinMode(9,OUTPUT); // set Pin9 as PUL
  pinMode(8,OUTPUT); // set Pin8 as DIR
  Serial.begin(9600);
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

    for(x = 0; x < steps; x++) // repeat 400 times a revolution when setting 400 on driver
    {
        digitalWrite(9,HIGH); // Output high
        delayMicroseconds(sleepMicroSeconds); // set rotate speed

        digitalWrite(9,LOW); // Output low
        delayMicroseconds(sleepMicroSeconds); // set rotate speed
    }
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
}
