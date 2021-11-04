#include <Kalman.h>

#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4IMU imu;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
//Create variables and assign values
double startVelocity=0;
double acceleration=0;
unsigned long t;
unsigned long tNow=0;
unsigned long tLast=0;
double traveledDistance=0;
double totalDistance=0;
double targetDistance=0.10;
int motorSpeedForward=100;
int motorSpeedStop=0;
int timeIncrement=10;
double accNow=0;
double accPrev1=0;
double accPrev2=0;
void setup() {
//Commands to setup imu and zumo robot
Serial.begin(9600);
Wire.begin();
imu.init();
imu.enableDefault();
delay(2000);
}

void loop() {
tLast=tNow; //sets time of last loop for caluculation of time differential
tNow=millis();//Sets new current time
t=tNow-tLast;//Sets time difference to time between loops
imu.read();//Reads from the imu
accPrev1=accNow;//Saves the previous acceleration
accPrev2=accPrev1;// Saves the acceleration before previous acceleration
accNow = imu.a.x;//Reads acceleration in x-direction
accNow= accNow *(9.81 / 16384); //Add conversion to m/s^2
acceleration = accNow / 2 + accPrev1 / 4 + accPrev2 / 4;//Averages the recent accelerations
traveledDistance=startVelocity*t+0.5*acceleration*(t*t);//Calculates the traveled distance since last loop 
totalDistance+=traveledDistance;//Adds the traveled distance the the total distance the robot has traveled so far
startVelocity+=(acceleration*t);//Updates the velocity of the robot for next loop
if (totalDistance<targetDistance)//Checks if the robot has moved the desired distance
{
  motors.setSpeeds(motorSpeedForward,motorSpeedForward);//If not then keep driving
}
else
{
  motors.setSpeeds(motorSpeedStop,motorSpeedStop);// If so then stop the robot
}
Serial.println("Reading:" + String(accNow));
Serial.println("Acc" + String(acceleration));
Serial.println("Total dist" + String(totalDistance));
Serial.println("Velocity" + String(startVelocity));


delay(timeIncrement);//Delay between loops
}
