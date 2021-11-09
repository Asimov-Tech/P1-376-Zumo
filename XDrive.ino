#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;


double xdist = 30;
double ydist = 10;
double count = 0;
bool XYdicration = true; //true for x og false for y
double EncoderCount = 0;



struct lokation {
  int xvalue;
  int yvalue;
};


void setup() {
  Serial.begin(9600);

}

void loop() {
  distCal();

}


void distCal() {
  if (XYdicration == true) {
    
    count = (((xdist*0.785) * 900) / (PI * 3, 9)); // 0,785 er en værdi der ganges med fordi det virker

    
    Serial.println((String)count);
    Serial.println("Encoder1 count = " + (String)EncoderCount);
    if (EncoderCount < count) {
      
      getEncoder();    

      motors.setSpeeds(105, 100);
    }
    else {
      Serial.println("Kør");
      motors.setSpeeds(0, 0);
    }

  }


}

void getEncoder() {
  int countsR= countsR += encoders.getCountsRight();
  int countsL= countsL += encoders.getCountsLeft();
  EncoderCount = ((countsR + countsL) / 2) ;  
  return EncoderCount;
  resetEncoders();
}
void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
