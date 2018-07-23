#include <PulsePosition.h>

// Simple loopback test: create 1 output to transmit
// test pulses, and 1 input to receive the pulses
PulsePositionOutput myOut(FALLING);
PulsePositionInput myIn(FALLING);

void setup() {
  myOut.begin(9);  // connect pins 9 and 10 together...
  myIn.begin(10);
  myOut.write(1, 1000); //throttle
  myOut.write(2, 2000); //aileron
  myOut.write(3, 2000); //elevator
  myOut.write(4, 1000); //rudder?
  myOut.write(5, 1500);
  myOut.write(6, 1500);
  myOut.write(7, 1500);
  myOut.write(8, 1500);
  myOut.write(9, 1500);
}

int count=0;

void loop() {
  int i, num;

  // Every time new data arrives, simply print it
  // to the Arduino Serial Monitor.
  //  num = myIn.available();
  //  if (num > 0) {
  //    count = count + 1;
  //    Serial.print(count);
  //    Serial.print(" :  ");
  //    #loop 
  //    for (i=1; i <= num; i++) {
  //      float val = myIn.read(i);
  //      Serial.print(val);
  //      Serial.print("  ");
  //    }
  //    Serial.println();
  //  }
}
