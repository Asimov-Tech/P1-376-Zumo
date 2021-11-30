#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;

#define NUM_SENSORS 5 //Number of activated sensors
uint16_t lineSensorValues[NUM_SENSORS]; //Some array that contains the raw read values from the sensors between 0-2000
bool useEmitters = true;
struct LineSensorsWhite { //Datatype that stores the boolean values for the sensorStates
  bool L;
  bool LC;
  bool C;
  bool RC;
  bool R;
};



bool left = false;
bool Center = false;
bool right  = false;



LineSensorsWhite sensorsState = {0, 0, 0};

int threshold[NUM_SENSORS] = {400, 400, 400, 400, 400};

void setup() {
  lineSensors.initFiveSensors();
  readSensors(sensorsState);
}

void loop() 
{ 
  
  
  driveDist(30);
  
}





void driveDist (double dist) {
  //resets encoders to avoid encoder counts for e.g. turns
  resetEncoders();
  //factor so that it can drive backwards
  int f = 1;
  if (dist < 0) f = -1;

  double count = dist * 78.8; //Don't ask, it just works

  //Following code prints on the lcd, the distance it is driving in cm
  lcd.gotoXY(0, 0);
  lcd.print("Driving");
  lcd.gotoXY(0, 1);
  lcd.print((String)dist + " cm");

  //actual function
  while (f * encoders.getCountsRight() < f * count) {

    readSensors(sensorsState);
    if (sensorsState.LC == true && sensorsState.C == true) 
    { 
      motors.setSpeeds(f * 100, f * 120);

    } 
    else if (sensorsState.C == true && sensorsState.RC == true) 
    {
      motors.setSpeeds(f*120,f*100);
    } 
    else 
    {
      motors.setSpeeds(f * 100, f * 100);
    }
  }
  //stops driving and clears LCD
  motors.setSpeeds(0, 0);
  lcd.clear();
}

void resetEncoders() 
{//Function to reset encoders
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}



void readSensors(LineSensorsWhite &state) {
  // Next line reads the sensor values and store them in the array lineSensorValues , aparameter passed by reference
  lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF);
  // In the following lines use the values of the sensors to update the struct
  sensorsState = {false, false, false};
  /*if ( lineSensorValues[0] < threshold[0])
    sensorsState.L = true;*/
  if ( lineSensorValues[1] < threshold[0])
    sensorsState.LC = true;
  if ( lineSensorValues[2] < threshold[1])
    sensorsState.C = true;
  if ( lineSensorValues[3] < threshold[0])
    sensorsState.RC = true;
  /*if ( lineSensorValues[4] < threshold[2])
    sensorsState.R = true;*/
}
