#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;


//necessary values for the loop to work 
bool XYdicration = true; //true for x og false for y
double EncoderCount = 0;
double count        = 0;



void setup() 
{
  Serial.begin(9600);

}

void loop() 
{
  distDrive(10,100);
  delay(1000);

}


void distDrive(double dist, double speeds) 
{
  resetEncoders();
  int f = 1;
  if (dist < 0) f = -1;
  if (XYdicration == true) 
  {

    count = dist*78.5; //Don't ask, it just works

    
    //Serial.println((String)count);
    //Serial.println("Encoder1 count = " + (String)EncoderCount);
    while(f*encoders.getCountsRight()< f*count) 
    {   
      motors.setSpeeds(f*speeds,f*speeds);
    }
      motors.setSpeeds(0, 0);    
      resetEncoders();       
   }
}

void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
