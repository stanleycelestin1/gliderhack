#include <PulsePosition.h>

// Simple loopback test: create 1 output to transmit
// test pulses, and 1 input to receive the pulses
PulsePositionOutput myOut(FALLING);
PulsePositionInput myIn(FALLING);

void setup() {
  myOut.begin(9);  // connect pins 9 and 10 together...
  myIn.begin(10);
  myOut.write(1, 1500); //throttle
  myOut.write(2, 1500); //aileron
  myOut.write(3, 1500); //elevator
  myOut.write(4, 1500); //rudder?
  myOut.write(5, 1500);
  myOut.write(6, 1500);
  myOut.write(7, 1500);
  myOut.write(8, 1500);
  myOut.write(9, 1500);
}

int count=0;
String inThrottleString = "";
String inAileronString  = "";
String inElevatorString = "";
String inRudderString   = "";

void loop() {
  int i, num, inChar,temp;
  inThrottleString = "";
  inAileronString  = "";
  inElevatorString = "";
  inRudderString   = "";

  
  // recieve serial input
  if (Serial.available()){
    num = Serial.available();
    Serial.print("Num is:");
    Serial.println(num);
    for ( i=1; i <= num; i++){
      // read 1 byte
      inChar = Serial.read();
      Serial.print("inChar is:");
      Serial.println(inChar);
      if (i==1 or i==2 or i==3 or i==4){
        // convert the incoming byte to a char and add it to the string
        inThrottleString+=(char)inChar;
      }
      else if (i==6 or i==7 or i==8 or i==9){
        // convert the incoming byte to a char and add it to the string
        inAileronString+=(char)inChar;
      }
      else if (i==11 or i==12 or i==13 or i==14){
        // convert the incoming byte to a char and add it to the string
        inElevatorString+=(char)inChar;
      }
      else if (i==16 or i==17 or i==18 or i==19){
        // convert the incoming byte to a char and add it to the string
        inRudderString+=(char)inChar;
      }
      else{
        continue;
      }
        
    }
    Serial.flush();
    
    Serial.println("New Data:");
    temp = map(inThrottleString.toInt(), 172, 1800, 1000, 2000);
    myOut.write(1, temp); //throttle
    Serial.println(temp);
    
    temp = map(inAileronString.toInt(), 172, 1800, 1000, 2000);
    myOut.write(2, temp); //throttle
    Serial.println(temp);

    temp = map(inElevatorString.toInt(), 172, 1800, 1000, 2000);
    myOut.write(3, temp); //throttle
    Serial.println(temp);

    temp = map(inRudderString.toInt(), 172, 1800, 1000, 2000);
    myOut.write(4, temp); //throttle
    Serial.println(temp);
    

    
  }
}
