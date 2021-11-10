#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
double x = 0; //the x coordinate
double y = 0; //the y coordinate
double pose = 0; //the position angle compared to the x-axis given in radians
double deg = 0; //For cenverting radians to degrees 
double temp;

double distRight = 10; //the dist traveled on the right wheel (right and left distance will always have to come for the same amount of time)
double distLeft = 7; //the dist traveled on the left wheel
//these should be found with the encoders 
double distBetweenWheels = 4;  //4 cm between each wheel

double poseVector[3] = {0,0,0};  // The "beginning" vector 
double newPose[3]; //A "new position" vector for new coordinates and angles from the distance traveled given that the starting position is [0,0,0]

void calCoordiPose(){
  double poseNew = 0; //Values for the "new position" vector
  double xNew = 0;
  double yNew = 0;
  double distTotal = 0; //Sort of a total distance traveled, but not really
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
  if (temp > M_PI){
    // pose = pose%M_PI; (If it spins more than 180 degrees in a single go, which it won't if we keep the amount of time to a minimum)
    temp = - M_PI + (temp - M_PI);
  }
  poseVector[0] =x; //Each new value is given to the position vector
  poseVector[1] =y;
  poseVector[2] =temp;
}

void convertRadians(){
  deg = poseVector[2]*(180/M_PI); //Just converting the radians to degrees 
  return deg;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  calCoordiPose();
  delay(200);
  convertRadians();
  delay(200);
  Serial.println("New x-value: " + (String)poseVector[0] + " New y-value: " + (String)poseVector[1] + " New position: " + (String)deg + " degrees");
  delay(5000); //Should do it quicker
  
}
