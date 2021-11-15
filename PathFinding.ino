#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
int i = 0;
int x,y;
int wildOats[2][7] = {{1,2,3,4,5,6,7},{2,5,7,9,4,6,3}};
int currentPose[2] = {x,y};
int moveThis[2] = {0,0};

void whereTo(){
  moveThis[0] = wildOats[0][i] - currentPose[0];
  moveThis[1] = wildOats[1][i] - currentPose[1];
  currentPose[0] = wildOats[0][i];
  currentPose[1] = wildOats[1][i];
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  for (i = 0; i < 7; i++){
    whereTo();
    Serial.println((String)currentPose[0] +" og "+ (String)currentPose[1]);
    delay(1000);
  }

}
