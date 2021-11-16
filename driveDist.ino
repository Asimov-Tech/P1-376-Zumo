
#include <Wire.h>
#include <Zumo32U4.h>

double count = 0;

void driveDist (double dist) {
  //resets encoders to avoid encoder counts for e.g. turns
  resetEncoders();
  //factor so that it can drive backwards
  int f = 1;
  if (dist < 0) f = -1;
    
    count = dist*78.5; //Don't ask, it just works
    
    //Following code prints on the lcd, the distance it is driving in cm
    lcd.gotoXY(0,0);
    lcd.print("Driving");
    lcd.gotoXY(0,1);
    lcd.print((String)dist + " cm");
   
    //actual function
    while(f*encoders.getCountsRight()< f*count) {
      
      motors.setSpeeds(f*100,f*100);//standard speed
      //if loop so that it avoids driving unevenly 
      if(f*encoders.getCountsRight()*1.01 > f*encoders.getCountsLeft()){
        // when right counter is higher, the left motor goes to 300 speed for a moment to even out
        motors.setSpeeds(f*300,f*100); 

      } else if (f*encoders.getCountsRight()*1.01 < f*encoders.getCountsLeft()){
        //Same as above but with other motor
        motors.setSpeeds(f*100,f*300);
      }
    } 
    //stops driving and clears LCD
    motors.setSpeeds(0, 0);
    lcd.clear();
}

void resetEncoders() {//Function to reset encoders
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
