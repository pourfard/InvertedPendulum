
#include <PID_v1.h>
#include <AccelStepper.h>
#define motorInterfaceType 1

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

int encoderPin1 = 2;
int encoderPin2 = 3;
int stepdelay =100;
int totalSteps = 0;
int maxSteps = 13000/2;
int myspeed = 12000;  //6400
int khalasi = 5;
int lastMSB = 0;
int lastLSB = 0;

long lastencoderValue = 0;

double Setpoint, Input, Output;

double Kp=0.39, Ki=0.6, Kd=0.0000001;
PID pid(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);



AccelStepper stepper = AccelStepper(motorInterfaceType, 9, 8);

void setup() {
  
//_______________________________motor______________________________________
  
  pinMode(9,OUTPUT); // set Pin9 as PUL
  pinMode(8,OUTPUT); // set Pin8 as DIR
  Serial.begin(9600);

//______________________________encoder_____________________________________
  
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);

//_____________________setting stepper motor________________________________

stepper.setMaxSpeed(100000);
stepper.setAcceleration(60000);


//_________________________________pid______________________________________

 pid.SetSampleTime(10);
pid.SetOutputLimits(-255,255);
  Input = encoderValue;
  Setpoint = 1.0;
 pid.SetMode(AUTOMATIC);
  
}

//____________________________driver motor________________________________

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
      
        //digitalWrite(8,HIGH); // set high level direction
       totalSteps+=steps;
    }
    else
    {
        //digitalWrite(8,LOW); // set high level direction
        totalSteps-=steps;
        
    }
   
   stepper.moveTo(totalSteps);
 stepper.runToPosition();
}


void doStep2(bool right, int steps, int sleepMicroSeconds)
{
    if(right)
    {
        digitalWrite(8,HIGH); // set high level direction
       totalSteps+=steps;
    }
    else
    {
        digitalWrite(8,LOW); // set high level direction
        totalSteps-=steps;
        
    }

    //if(totalSteps>maxSteps || totalSteps<-maxSteps){
    //  return;
    //}
    for(int x = 0; x < steps; x++)
    {
        digitalWrite(9,HIGH); // Output high
        delayMicroseconds(1); // set rotate speed
        digitalWrite(9,LOW); // Output low
        //delayMicroseconds(1); // set rotate speed
        delayMicroseconds(sleepMicroSeconds); // set rotate speed
    }
}

//___________________________________________________________________________________

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time

}

void loop() {

//__________________________contorol pid_______________________________

    if (encoderValue >700 && encoderValue<1900)
    {
            Input = encoderValue;
            Input /= 1200.0;
           
  pid.Compute();
  Serial.println("Output "+String(Output*myspeed)+" "+ String(Input));

    if (encoderValue >(1200-khalasi) && encoderValue<(1200+khalasi))
      Output = 0;
    if(Output > 0)
      doStep(false,(int)abs(myspeed*Output),0);
     delayMicroseconds(1);
    if(Output < 0)
      doStep(true, (int)abs(myspeed*Output),0);
      delayMicroseconds(1); 
    }

//_____________________________Serial____________________________________

        if(Serial.available() > 0) {
      String incomingString = Serial.readStringUntil('\r');
      String dir = getValue(incomingString, ',', 0);//0
      String steps = getValue(incomingString, ',', 1);//1
      String sleepMicroSeconds = getValue(incomingString, ',', 2);//2
      doStep(dir == "right", steps.toInt(), sleepMicroSeconds.toInt());
      Serial.println("#"+String(encoderValue)+"#");
  }

    Serial.println("#"+String(encoderValue)+"#");


}
