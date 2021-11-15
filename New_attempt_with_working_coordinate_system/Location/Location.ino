#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
double x = 0, y = 0, pose = 0, temp, deg = 0; //the x coordinate, the y coordinate, the position angle compared to the x-axis given in radians, "deg" For cenverting radians to degrees 
//Distance on the right and left will always come from an equal amount of time
double distRight, distLeft; //the dist traveled on the right wheel, dist traveled on left wheel, counts from 

double poseVector[3] = {0,0,0};  // The "beginning" vector 
double newPose[3]; //A "new position" vector for new coordinates and angles from the distance traveled given that the starting position is [0,0,0]
int counts[2]; //an array for the numbers retrieved from the encoders, not really needed here, but maybe in some other part of the code idk

void getEncoderAndDist() {
  counts[0] = encoders.getCountsAndResetRight(); //Getting the counts from each encoder
  counts[1] = encoders.getCountsAndResetLeft();
  distRight = (1/78.5)*counts[0]; // Using these counts to calculate the distance each wheel has traveled 
  distLeft = (1/78.5)*counts[1]; // 1/78.5 is the ratio between a centimeter and 1 encoder count
  return counts;
}

void calCoordiPose(){
  double poseNew = 0; //Values for the "new position" vector
  double xNew = 0;
  double yNew = 0;
  double distTotal = 0; //Sort of a total distance traveled, but not really
  double distBetweenWheels = 8.5;  //4 cm between each wheel
  poseNew = (distRight-distLeft)/distBetweenWheels; //calculate the new position angle after the wheels have moved their distance, given in radians
  distTotal = (distRight+distLeft)/2; //calculating total distance so that a nex x and y can be calculated
  xNew = distTotal*cos(poseVector[2]+(poseNew/2)); //calculate the new x-coordinate after move
  yNew = distTotal*sin(poseVector[2]+(poseNew/2)); // calculate the new y-coordinate after move

  newPose[0] = xNew; //Each new calculated value is given to the "new position" vector
  newPose[1] = yNew;
  newPose[2] = poseNew;
  
  x = poseVector[0] + newPose[0]; // adding new x-value to the old one
  y = poseVector[1] + newPose[1]; // adding new y-value to the old one
  temp = poseVector[2] + newPose[2]; // adding new pose-value to the old one (calling it "temp" because otherwise it wasn't working)
   //These new values are now the values of the next "beginning" vector

   //The next two if-statements make sure that when the rotation angel exceeds 180 or goes below -180 degrees, it will "reset" it
  if (temp > M_PI){
    // pose = pose%M_PI; (If it spins more than 180 degrees in a single go, which it won't if we keep the amount of time to a minimum)
    temp = - M_PI + (temp - M_PI);
  }
  if (temp < -M_PI){
    temp = M_PI -(temp + M_PI);
  }
  poseVector[0] =x; //Each new value is given to the position vector
  poseVector[1] =y;
  poseVector[2] =temp;
}

void convertRadians(){
  deg = poseVector[2]*(180/M_PI); //Just converting the radians to degrees 
  return deg;
}

void location(){ //function compiling all the other functions
  getEncoderAndDist(); //Getting the distance each wheel has traveled from the encoder counts 
  calCoordiPose(); //using those distances to calculate the coordinates
  convertRadians(); // converting the radians to degrees for the print
  Serial.println("New x-value: " + (String)poseVector[0] + " New y-value: " + (String)poseVector[1] + " New position: " + (String)deg + " degrees");
  delay(50); //Reading distances in intervals of 50 milliseconds

}



void setup() {
  Serial.begin(9600);
}
//bool testing = true;

void loop() {
  location();
 // delay(3000);
 // motors.setSpeeds(50,50);
 // while (testing == true){
 // if ((poseVector[0]+0.4) >= 10){
 //   testing = false;
 // }
  }
  //motors.setSpeeds(0,0);
  
