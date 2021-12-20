#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
#include <Zumo32U4IMU.h>
#include <Arduino.h>
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;

const int numDest = 15; //Number of destinations
double x = 0, y = 0, pose = 0, temp, deg = 0; //the x coordinate, the y coordinate, the position angle compared to the x-axis given in radians, "deg" For cenverting radians to degrees 
//Distance on the right and left will always come from an equal amount of time
double distRight, distLeft; //the dist traveled on the right wheel, dist traveled on left wheel, counts from 
int Position = 0; // The angle at which the robot is pointing in the global coordinate system

double poseVector[3] = {0,0,0};  // The "beginning" vector 
double newPose[3]; //A "new position" vector for new coordinates and angles from the distance traveled given that the starting position is [0,0,0]
int counts[2]; //an array for the numbers retrieved from the encoders
int rememberCounts[2] = {0,0}; //a vector to remember the counts 

int i = 0; //Iterations of the for-loop
int maxY = 90; //The max y-value of the field
int wildOats[2][numDest];//An array describing the positions of the wild oats


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

int threshold[NUM_SENSORS] = {400, 40, 400, 400, 400};


void returnYmax(){ //Drives the distance needed to go from current y-position to the max y-value on the field 
  driveDist(maxY - poseVector[1]);
  resetCounts();//Reseting the encoder counts as to not confuse the "location" function
}

void returnTo0(){ //Driving the distance needed to go from the current y-position to the x-axis
  driveDist(-(0-wildOats[1][i+1]));
  resetCounts(); //Reseting the encoder counts as to not confuse the "driveDist" function, didn't work when I put it in the function
}

void continuing(){
  if ((wildOats[0][i+1])<wildOats[0][i]){ //If the next point's x-value is less than the current one
    driveDist((wildOats[0][i]-wildOats[0][i+1])); //Drive to the x-coordinate of the wild oat
  }
  else{ //If the next point's x-value is higher thann the current one.
    driveDist(wildOats[0][i+1]-wildOats[0][i]); //Drive to the x-coordinate of the wild oat, just calculated differently 
  }
  resetCounts();
  delay(100);
  if (wildOats[1][i+1] != maxY && wildOats[1][i+1] != 0){ //if-statement that determines wether the robot is on a spot that is on y = 0 or y = maxY
  //If-statement that determines wether the robot should turn right or left before driving on the y-axis
  if (poseVector[1] > (maxY-5) && poseVector[1] < (maxY+5)){ //This depends on the wethere the robot is on the max y-value or not
    turnRight();
  }
  else {
    turnLeft();
  }
  }
  }

void driveToSpot(){ //Function driving the robot to the y-value of the wild oat
  if(poseVector[1] <(maxY+5) && poseVector[1] > (maxY-5)){ //Determines same thing as the second if-statement in "continuing"
    driveDist(maxY-wildOats[1][i+1]); //Then drives to the wild oats y-value
  }
  else{
    driveDist(wildOats[1][i+1]);
    }
    resetCounts();
    delay(100);
    //turnStraight(); //This will be used when the robot has to drive out in the field
}


void sameLine(){ //This funtion is used when the next point is on the same line as the current one
  if (wildOats[1][i+1] < wildOats[1][i+2]){ //If the next point's y-value is larger than the current one
    turnLeft(); //it turns left
    driveDist(wildOats[1][i+2]-wildOats[1][i+1]); //then drives to that point
  }
  else { //if not
    turnRight(); //it turns right
    driveDist(wildOats[1][i+1]-wildOats[1][i+2]); //and then drives to the point
    }
    i++; //Because is goes to a new point withour going through the entire vood loop
    buzzer.playFrequency(1000,500,10); //Buzzer to tell that it has hit a point
 }
  

void returning(){ //A function that returns the robot to either the x-axis or the max y-value of the field
  //depending on wether it is closer to the max y-value or the x-axis
  double xRoute = wildOats[1][i+1] + wildOats[1][i+2];
  double yRoute = (maxY-wildOats[1][i+1]) +(maxY-wildOats[1][i+2]);
  if (wildOats[1][i+1] == maxY || wildOats[1][i+1] == 0){ //If the next point is either on max y-value or x-axis
    delay(1); //aka "do nothing"
  }
   else{ //If not, it uses the distances calculated earlier as x and y Route, to determine which route to take to the next point.
  if (xRoute >= yRoute){ //If the x Route is longer, it will go the y route
    turnLeft();
    returnYmax(); //This function just drives the distance between the current point and the max y-value
  }
  else { //If the y route is longer, it will go the x route
   turnRight();
   returnTo0(); //This function just drives the distance between the current point and the x-axis
  }
  }
  if (wildOats[0][i+2]<wildOats[0][i+1]){ //Determining which way to turn depending on whether the x-value of the next point is bigger or smaller
    turnBackward();}
    else{
    turnStraight();}
  }


void readSensors(LineSensorsWhite &state) {
  // Next line reads the sensor values and store them in the array lineSensorValues , aparameter passed by reference
  lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF);
  // In the following lines use the values of the sensors to update the struct
  sensorsState = {false, false, false};
  /*if ( lineSensorValues[0] < threshold[0])
    sensorsState.L = true;*/
  if ( lineSensorValues[1] < threshold[0]){
    sensorsState.LC = true;
    //Serial.println("L = true");
    }
  if ( lineSensorValues[2] < threshold[1]){
    sensorsState.C = true;
    //Serial.println("C = true");
    }
  if ( lineSensorValues[3] < threshold[0]){
    sensorsState.RC = true;
    //Serial.println("R = true");
    }
  /*if ( lineSensorValues[4] < threshold[2])
    sensorsState.R = true;*/
}




void driveDist (double dist) {
  double count = 0;
  //resets encoders to avoid encoder counts for e.g. turns
  //factor so that it can drive backwards
  int f = 1;
  if (dist < 0) f = -1;
    
    count = dist*78.8; //Don't ask, it just works
    
    
    //Following code prints on the lcd, the distance it is driving in cm
    lcd.gotoXY(0,0);
    lcd.print("Driving");
    lcd.gotoXY(0,1);
    lcd.print((String)dist + " cm");
   
    //actual function
    while(f*rememberCounts[0]< f*count) {
      location();
      readSensors(sensorsState);
      //if loop so that it avoids driving unevenly by using line sensors
      if(sensorsState.C == true && sensorsState.RC == true){ //f*rememberCounts[0]*1.001 > f*rememberCounts[1]
        //When both the center and right sensor can see white, it turns a little left
        motors.setSpeeds(f*160,f*100); 
        delay(1);

      } else if (sensorsState.LC == true && sensorsState.C == true){ //f*rememberCounts[0]*1.001 < f*rememberCounts[1]
        //When both the center and left sensor can see white, it turns a little right
        motors.setSpeeds(f*100,f*160);
        delay(1);
      }
      else if (sensorsState.LC == true){ //f*rememberCounts[0]*1.001 < f*rememberCounts[1]
        //If the only the left sensor can see white, it turns faster
        motors.setSpeeds(f*100,f*180);
        delay(1);}
        
        else if (sensorsState.RC == true){ //f*rememberCounts[0]*1.001 < f*rememberCounts[1]
        //If the only the right sensor can see white, it turns faster
        motors.setSpeeds(f*180,f*100);
        delay(1);}
      else {
        motors.setSpeeds(f*100,f*100);//standard speed
      }
    }
    //stops driving and clears LCD
    motors.setSpeeds(0, 0);
    lcd.clear();
}

void resetCounts() {//Function to reset an array that counts the encoders, this is because the encoders are also needed to calculate the location function
  rememberCounts[0] = 0;
  rememberCounts[1] = 0;
}
