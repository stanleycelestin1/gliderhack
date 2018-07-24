/*
DLG Control Uplink Sketch
iskushan

based on:
SBUS_example.ino
Brian R Taylor
brian.taylor@bolderflight.com
2016-09-21

Copyright (c) 2016 Bolder Flight Systems

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// This example reads an SBUS packet from an
// SBUS receiver (FrSky X8R) and then takes that
// packet and writes it back to an SBUS
// compatible servo. The SBUS out capability (i.e.
// writing a command to the servo) could be generated
// independently; however, the packet timing would need
// to be controlled by the programmer, the write function
// simply generates an SBUS packet and writes it to the
// servos. In this case the packet timing is handled by the
// SBUS receiver and waiting for a good packet read.

#include "SBUS.h"
#include "Servo.h"

// a SBUS object, which is on Teensy hardware
// serial port 1
SBUS x8r(Serial1);
Servo RightAileron;  // create servo object to control a servo
Servo LeftAileron;  // create servo object to control a servo
Servo Elevator;  // create servo object to control a servo
int rightaileron_servoval = 90;
int elevator_servoval = 90;

// channel, fail safe, and lost frames data
uint16_t channels[16];
uint8_t failSafe;
uint16_t lostFrames = 0;

void setup() {
  // begin the SBUS communication
  x8r.begin();
  RightAileron.attach(5); //right aileron servo
  LeftAileron.attach(6); //left aileron servo
  Elevator.attach(23); //left aileron servo
  RightAileron.write(rightaileron_servoval);
  LeftAileron.write(180 - rightaileron_servoval);
  Elevator.write(90);   
  Serial.begin(115200);
  Serial.println("receiving control commands");
  pinMode(5,OUTPUT);
}

void loop() {
  // look for a good SBUS packet from the receiver
  if(x8r.read(&channels[0], &failSafe, &lostFrames)){
    // write the SBUS packet to an SBUS compatible servo
    //x8r.write(&channels[0]);
    //digitalWrite(5,HIGH);
    //delay(1);
    //digitalWrite(5,LOW);
    
    Serial.print("t : ");
    Serial.println(channels[0]); //throttle
    Serial.print("a : ");
    Serial.println(channels[1]); //aileron
    rightaileron_servoval = map(channels[1], 172, 1800, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
    RightAileron.write(rightaileron_servoval);
    LeftAileron.write(rightaileron_servoval);    
    Serial.print("e : ");
    Serial.println(channels[2]); //elevator
    elevator_servoval = map(channels[2], 172, 1800, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
    Elevator.write(elevator_servoval);
    Serial.print("r : ");
    Serial.println(channels[3]); //rudder
    Serial.println("~~~~~~~~");
    
  }
}

