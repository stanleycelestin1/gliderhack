/*
 * DLG Telemetry Downlink Example
 * <iskushan>
 * based on:
  FrSky S-Port Telemetry library example
  (c) Pawelsky 20160916
  Not for commercial use
  
  Note that you need Teensy 3.x/LC or 328P based (e.g. Pro Mini, Nano, Uno) board and FrSkySportTelemetry library for this example to work
*/

/*
 * DEFINES
 */
 
#define TELEMETRY_TIMER         0
#define LED_TIMER               1
#define UPLINK_TIMER            2

#define TELEMETRY_TIME_INTERVAL 250
#define UPLINK_TIME_INTERVAL    50
#define LED_TIME_INTERVAL       1000

// Uncomment the #define below to enable internal polling of data.
// Use only when there is no device in the S.Port chain (e.g. S.Port capable FrSky receiver) that normally polls the data.
//#define POLLING_ENABLED

#include "FrSkySportSensor.h"
#include "FrSkySportSensorAss.h"
#include "FrSkySportSensorFcs.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSensorGps.h"
#include "FrSkySportSensorRpm.h"
#include "FrSkySportSensorSp2uart.h"
#include "FrSkySportSensorVario.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportTelemetry.h"
#include "Timers.h"
// uplink include
#include "SBUS.h"
#include "Servo.h"
//#include "Timers.cpp"

#if !defined(__MK20DX128__) && !defined(__MK20DX256__) && !defined(__MKL26Z64__) && !defined(__MK66FX1M0__) && !defined(__MK64FX512__)
#include "SoftwareSerial.h"
#endif

FrSkySportSensorAss ass;                               // Create ASS sensor with default ID
FrSkySportSensorFcs fcs;                               // Create FCS-40A sensor with default ID (use ID8 for FCS-150A)
FrSkySportSensorFlvss flvss1;                          // Create FLVSS sensor with default ID
FrSkySportSensorFlvss flvss2(FrSkySportSensor::ID15);  // Create FLVSS sensor with given ID
FrSkySportSensorGps gps;                               // Create GPS sensor with default ID
FrSkySportSensorRpm rpm;                               // Create RPM sensor with default ID
FrSkySportSensorSp2uart sp2uart;                       // Create SP2UART Type B sensor with default ID
FrSkySportSensorVario vario;                           // Create Variometer sensor with default ID
#ifdef POLLING_ENABLED
  FrSkySportTelemetry telemetry(true);                 // Create telemetry object with polling
#else
  FrSkySportTelemetry telemetry;                       // Create telemetry object without polling
#endif

//constants
const int ledPin = 13;

//Flags

bool LED_STATE = 0;

//
//
// a SBUS object, which is on Teensy hardware
// serial port 1
SBUS x8r(Serial1);
Servo RightAileron;  // create servo object to control a servo
Servo LeftAileron;  // create servo object to control a servo
Servo Elevator;  // create servo object to control a servo
int aileron_servoval = 90;
int elevator_servoval = 90;
int rudder_servoval = 90;

// channel, fail safe, and lost frames data
uint16_t channels[16];
uint8_t failSafe;
uint16_t lostFrames = 0;
//
//

//function prototypes
void downlinkTelemetry();
void RespLEDTimerExpired(void);
unsigned char TestLEDTimerExpired(void);
void RespTelemetryTimerExpired(void);
unsigned char TestTelemetryTimerExpired(void);

int throttle, aileron, elevator,rudder;
void setup()
{
    // Configure the telemetry serial port and sensors (remember to use & to specify a pointer to sensor)
  #if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)
    telemetry.begin(FrSkySportSingleWireSerial::SERIAL_3, &ass, &fcs, &flvss1, &flvss2, &gps, &rpm, &sp2uart, &vario);
  #else
    telemetry.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_12, &ass, &fcs, &flvss1, &flvss2, &gps, &rpm, &sp2uart, &vario);
  #endif

  pinMode(ledPin,OUTPUT);
    
  //initialize the timers
  TMRArd_InitTimer(TELEMETRY_TIMER , TELEMETRY_TIME_INTERVAL);
  TMRArd_InitTimer(UPLINK_TIMER, UPLINK_TIME_INTERVAL);
  TMRArd_InitTimer(LED_TIMER, LED_TIME_INTERVAL);
  //
  //

  //
  //
  // begin the SBUS communication
  x8r.begin();
  RightAileron.attach(5); //right aileron servo
  LeftAileron.attach(6); //left aileron servo
  Elevator.attach(23); //left aileron servo
  RightAileron.write(aileron_servoval);
  LeftAileron.write(aileron_servoval);
  Elevator.write(90);   
  Serial.begin(115200);
  Serial.println("receiving control commands");
  pinMode(5,OUTPUT);

  //
  //
  //
}

void loop()
{ 
  //send telemetry
  downlinkTelemetry();
  
  //run the loop
  if (TestTelemetryTimerExpired()) RespTelemetryTimerExpired();

  //blink the led
  if (TestLEDTimerExpired()) RespLEDTimerExpired();

  if(x8r.read(&channels[0], &failSafe, &lostFrames)){
    throttle = channels[0];
    aileron  = channels[1];
    elevator = channels[2];
    rudder   = channels[3];
    
    Serial.print("t : ");
    Serial.println(throttle); //throttle
    Serial.print("a : ");
    Serial.println(aileron); //aileron
    Serial.print("e : ");
    Serial.println(elevator); //elevator
//    Serial.print("a : ");
//    Serial.println(rudder); //rudder
    
  }
  // uplink
  if (TestUplinkTimerExpired()) RespUplinkTimerExpired();

}

void downlinkTelemetry(){
    // Set airspeed sensor (ASS) data
  ass.setData(76.5);  // Airspeed in km/h

  // Set current/voltage sensor (FCS) data
  // (set Voltage source to FAS in menu to use this data for battery voltage,
  //  set Current source to FAS in menu to use this data for current readins)
  fcs.setData(1.0,   // Current consumption in amps
              4.2);  // Battery voltage in volts

  // Set LiPo voltage sensor (FLVSS) data (we use two sensors to simulate 8S battery 
  // (set Voltage source to Cells in menu to use this data for battery voltage)
  //flvss1.setData(4.07, 4.08, 4.09, 4.10, 4.11, 4.12);  // Cell voltages in volts (cells 1-6)
  //flvss2.setData(4.13, 4.14);                          // Cell voltages in volts (cells 7-8)

  // Set GPS data
  gps.setData(48.858289, 2.294502,   // Latitude and longitude in degrees decimal (positive for N/E, negative for S/W)
              1000.00,                 // Altitude in m (can be negative)
              100.0,                 // Speed in m/s
              90.23,                 // Course over ground in degrees (0-359, 0 = north)
              18, 2, 9,             // Date (year - 2000, month, day)
              12, 00, 00);           // Time (hour, minute, second) - will be affected by timezone setings in your radio

  // Set RPM/temperature sensor data
  // (set number of blades to 2 in telemetry menu to get correct rpm value)
  rpm.setData(200,    // Rotations per minute
              25.6,   // Temperature #1 in degrees Celsuis (can be negative, will be rounded)
              -7.8);  // Temperature #2 in degrees Celsuis (can be negative, will be rounded)

  // Set SP2UART sensor data
  // (values from 0.0 to 3.3 are accepted)
  sp2uart.setData(1.5,    // ADC3 voltage in volts
                  3.3);   // ADC4 voltage in volts

 // Set variometer data
  // (set Variometer source to VSpd in menu to use the vertical speed data from this sensor for variometer).
  vario.setData(1000.00,  // Altitude in meters (can be negative)
                -1.5);  // Vertical speed in m/s (positive - up, negative - down)
  // Send the telemetry data, note that the data will only be sent for sensors
  // that are being polled at given moment
  telemetry.send();

}

//LED timer functions
void RespLEDTimerExpired(void) {
  LED_STATE = !LED_STATE;
  digitalWrite(ledPin, LED_STATE);
  Serial.println("LED");
  TMRArd_InitTimer(LED_TIMER, LED_TIME_INTERVAL);
}

unsigned char TestLEDTimerExpired(void) {
  return (unsigned char)(TMRArd_IsTimerExpired(LED_TIMER));
}

//telemetry timer functions
void RespTelemetryTimerExpired(void) {
  //downlinkTelemetry();
  Serial.println("telemetry");
  TMRArd_InitTimer(TELEMETRY_TIMER, TELEMETRY_TIME_INTERVAL);
}

unsigned char TestTelemetryTimerExpired(void) {
  return (unsigned char)(TMRArd_IsTimerExpired(TELEMETRY_TIMER));
}


//uplink timer functions
void RespUplinkTimerExpired(void) {
 
  Serial.println("uplink");
  uplink();
  TMRArd_InitTimer(UPLINK_TIMER, UPLINK_TIME_INTERVAL);
}

unsigned char TestUplinkTimerExpired(void) {
  return (unsigned char)(TMRArd_IsTimerExpired(UPLINK_TIMER));
}






void  uplink(){
    

  aileron_servoval = map(aileron, 172, 1800, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  RightAileron.write(aileron_servoval);
  LeftAileron.write(aileron_servoval);    
  
  elevator_servoval = map(elevator, 172, 1800, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  Elevator.write(elevator_servoval);

//  rudder_servoval = map(rudder, 172, 1800, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
//  Rudder.write(rudder_servoval);
  
}

