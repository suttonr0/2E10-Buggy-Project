volatile int ir_interrupt = 0; 
const int irPin = 2;//input from IR gantry
const int moPin = 3;//output to motor
String inputString = "";
bool stringComplete = false;
bool moving = false;
bool sent = false;
int count = 0;
//for UltraSonics
const int pingPin = A4;
const int Vcc = A3;
const int Gnd = A2;
const int testPin = 13;
unsigned long previousMillis = 0;
int interval = 1000;
bool foundObst = false;
const int LEDPin = 13;//output to motor

void setup()
{
Serial.begin(9600);
Serial.print("+++");
delay(1100); 
Serial.println("ATID 6869, CH C, CN");

SendPulse_MoPin(4);

pinMode(irPin, INPUT);//setting the irpin as an input (ie it's receiving info)
pinMode(moPin, OUTPUT);//setting mopin as an output (ie it's sending info (turning motors on/off)
attachInterrupt(0, LEDISR, RISING);//anytime there is a rising edge it calls LEDISR function
//attachInterrupt(pin number, function to use, what the signal is(LOW, CHANGE, RISNG, FALLING)

}
void loop()
{
  
  if (stringComplete == true && ir_interrupt == 0)//basically this means that if the buggy has received a message but hasn't reached a gantry yet (ie the buggy hasn't begun yet)
  {
    //Serial.write("start");
    if(inputString.indexOf("b1bg") > -1)//if control is telling the buggy to begin, then it will start moving
    {
       Serial.write("b1bg");
       SendPulse_MoPin(4);
       moving = true;
       stringComplete = false;
       inputString = "";
    }
       if (inputString.indexOf("b1st") > -1)
      {

        Serial.write("b1st");
        SendPulse_MoPin(2);
        moving = false;
        stringComplete = false;
        inputString = "";
       
      }

        if(inputString.indexOf("b1fg") > -1)//if control is telling the buggy to begin, then it will start moving
    {
       Serial.write("b1fg");
        
       SendPulse_MoPin(4);
       moving = true;
       stringComplete = false;
       inputString = "";
    }
  }

  
  if(ir_interrupt==1)//if it is at a gantry
  {
    //int duration=pulseIn(irPin, HIGH);
    //String gantry=gantry_Detection(duration);//find which gantry from the legth of pulse
    if (!sent)
    {
      int duration=pulseIn(irPin, HIGH);
      String gantry=gantry_Detection(duration);//find which gantry from the legth of pulse
      Serial.print("b1g" + gantry);//tell control what gantry the buggy is at
      delay (1000);
      sent = true;
    }
    if (stringComplete == true)//if there is a message from control
    {
     // Serial.print(inputString);
      if (inputString.indexOf("b1go") > -1)//if command is telling buggy to move
      {
         Serial.print("b1mo");
         SendPulse_MoPin(4);
         //sends pulse of 4ms which causes buggy to move
         moving = true;
         ir_interrupt = 0;//as buggy is leaving it will no longer be detecting that gantry
         sent = false;
         inputString = "";
      }
        
        if (inputString.indexOf("b1wt") > -1){
          
        int duration=pulseIn(irPin, HIGH);
        String gantry=gantry_Detection(duration);
        delay(1500);
        Serial.print("b1g" + gantry); //Keep telling control what gantry it is at;
        sent = true;
        inputString = "";
        }
        
      if (inputString.indexOf("b1pk") > -1)//if command is telling buggy to move
      {
        Serial.print("b1mo");
        SendPulse_MoPin(8);

        
        delay(3000);
        SendPulse_MoPin(4);
        delay(4700);
        SendPulse_MoPin(2);
        Serial.print("b1pk");

        moving = false;
        sent = false;
        inputString = "";
      }
      stringComplete = false;
    }
  }

  //ULTRASONICS START
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval)//checking every second
  { 
     // save the last time you blinked the LED
     previousMillis = currentMillis;
     if(obstacleDetection())
     {
        if(foundObst == false)
        Serial.print("b1ob");
        
        SendPulse_MoPin(2);
        foundObst = true;
        digitalWrite(LEDPin, LOW);//making it LOW
        digitalWrite(LEDPin,HIGH);
        delay(200);
        digitalWrite(LEDPin, LOW);
     }
     else
     {
        if(foundObst == true)
        Serial.print("b1cl");
        
        foundObst = false;
        //SendPulse_MoPin(4);
     }
  }
  //ULTRASONICS END
}


String gantry_Detection(int d)
{
 
   if (d > 500 && d < 1500)//if the duration is a second (or thereabouts) then we know it's at gantry1
  {
    return "1";
  }
  else if (d > 1500 && d < 2500)
  {
    return "2";
  }
  else if (d > 2500 && d < 3500)
  {
    return "3";
  }
}

boolean obstacleDetection()
{
  long dist = 0; 
  pinMode(pingPin, OUTPUT);//turning pingPin to an output
  pinMode(Vcc, OUTPUT);
  pinMode(Gnd, OUTPUT);
  digitalWrite(Vcc, HIGH);
  digitalWrite(Gnd, LOW);
  
  digitalWrite(pingPin, LOW);//making it LOW
  delayMicroseconds(2);//runs HIGH for 10 micro seconds
  digitalWrite(pingPin,HIGH);//this is sending the initiation pulse
  delayMicroseconds(10);//runs HIGH for 10 micro seconds
  digitalWrite(pingPin, LOW);//puts it back to LOW
  
  pinMode(pingPin, INPUT);//Now the pingPin is set as an INPUT
  long pulse = pulseIn(pingPin, HIGH);//returns the length of time that the distance pulse has been HIGH for
  //possible timeout (stop measuring and ignore pulses that are above 15cm)

  dist = pulse/58.138;
  
  if (dist <= 20)
    return true; 
  else
    return false;
}

void LEDISR() //this is for when the buggy is at a gantry.
{
  if (moving)
  {
    ir_interrupt=1;
    SendPulse_MoPin(2);
    //sends pulse of 2ms which stops both motors 
    moving = false;
  }
 
}

void serialEvent() //This event handler was found online on an arduino forum (or the arduino website, I can't remember which!)
{
  while (Serial.available()) {
    // get the new byte:
   char inChar = (char)Serial.read();
    //Serial.print(inChar);
    // add it to the inputString:
    if (inChar != '\n')
    {
      inputString += inChar;
    }
    // if the incoming character is a newline, set a flag
    // for the main loop
    if (inChar == '\n') {
      //Serial.print(inputString);
      stringComplete = true;
    }
  }
}


void SendPulse_MoPin(int x)
{
    digitalWrite(moPin, LOW);
    delay(2);
    digitalWrite(moPin, HIGH);
    delay(x);
    digitalWrite(moPin, LOW);
    delay(2);
    
}






