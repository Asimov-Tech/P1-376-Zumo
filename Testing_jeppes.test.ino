#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;


double xdist = 10;
double ydist = 10;
double count = 0;
double EncoderCount = 0;




/* turnAngle is a 32-bit unsigned integer representing the amount
  the robot has turned since the last time turnSensorReset was
  called.  This is computed solely using the Z axis of the gyro, so
  it could be inaccurate if the robot is rotated about the X or Y
  axes.

  Our convention is that a value of 0x20000000 represents a 45
  degree counter-clockwise rotation.  This means that a uint32_t
  can represent any angle between 0 degrees and 360 degrees.  If
  you cast it to a signed 32-bit integer by writing
  (int32_t)turnAngle, that integer can represent any angle between
  -180 degrees and 180 degrees. */
uint32_t turnAngle = 0;
int turnAngle2 = turnAngle;
// turnRate is the current angular rate of the gyro, in units of
// 0.07 degrees per second.
int16_t turnRate;

// This is the average reading obtained from the gyro's Z axis
// during calibration.
int16_t gyroOffset;

// This variable helps us keep track of how much time has passed
// between readings of the gyro.
uint16_t gyroLastUpdate = 0;

//The amount of degrees the robot has turned
int turnAngleDegrees;


bool xdriven        = false;
bool ydriven        = false;
bool turnedminus90  = false;
bool turned90       = false;



void setup() 
{
  Serial.begin(9600);
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  lcd.clear();
  delay(500);
  
}

void loop() 
{
  turnSensorUpdate();
  turnAngleDegrees = ((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.gotoXY(0, 0);
  lcd.print((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.print(F("   "));


  
  checkxdriven();
  
  checkturn90();
  getEncoder();
  checkydriven();
  

}







void checkxdriven()
{
  if(xdriven==false)
  {
    owndistcalx();
    delay(100);
  }
}


void checkturn90()
{
  if(xdriven==true)
  {
    if (turnAngleDegrees >= 90 && 91 >= turnAngleDegrees)
    {
      motors.setSpeeds(0, 0);
      resetEncoders();
      turned90=true;
    }
    else
    {
      motors.setSpeeds(-150, 150);
    }
  }
}




void checkydriven()
{
  if(turned90==true)
  {
    owndistcaly();
    ydriven=true;
  }
}







void owndistcalx()
{
  int countdist = (((xdist * 0.788) * 900) / (PI * 3, 9));
  if(EncoderCount < countdist)
  {
    getEncoder();  
    motors.setSpeeds(100,100);
  }
  else
  {
    motors.setSpeeds(0,0);
    xdriven=true;
  }
}



void owndistcaly()
{
  int countdist = (((ydist * 0.788) * 900) / (PI * 3, 9));
  if(EncoderCount < countdist)
  {
    getEncoder();
    motors.setSpeeds(100,100);
  }
  else
  {
    motors.setSpeeds(0,0);
    ydriven=true;    
  }
}

/*
void distCal() 
{
  if (XYdicration == true) 
  {
    // dictance formlen omskrives for at få at finde hvor mange "counts der skal til på en distance
    count = (((xdist * 0.788) * 900) / (PI * 3, 9)); // 0,785 er en værdi der ganges med fordi det virker
    if (EncoderCount < count) 
    {
      getEncoder();
      motors.setSpeeds(100, 110);// lav til funktion
    }
    else 
    {
      motors.setSpeeds(0, 0);
    }
  }
}
*/

void getEncoder() 
{
  int countsR = countsR += encoders.getCountsRight();
  int countsL = countsL += encoders.getCountsLeft();
  EncoderCount = ((countsR + countsL) / 2) ;
  Serial.println(EncoderCount);
  return EncoderCount;
}

void resetEncoders() 
{
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}



//This function makes the robot inbetween 90 and 91 degrees
void turn90()
{
  
  if (turnAngleDegrees >= 90 && 91 >= turnAngleDegrees)
  {
    motors.setSpeeds(0, 0);
    resetEncoders();
  }
  else
  {
    motors.setSpeeds(-120, 100);
  }
}



void turnminus90()
{
  
  if (turnAngleDegrees <= -90 && -91 <= turnAngleDegrees)
  {
    motors.setSpeeds(0, 0);
  }
  else
  {
    motors.setSpeeds(100, -120);
  }
}

//Ik rør ved noget herefter, dette er alt sammen Carlos kode der bliver brugt til gyro
/* This should be called in setup() to enable and calibrate the
  gyro.  It uses the LCD, yellow LED, and button A.  While the LCD
  is displaying "Gyro cal", you should be careful to hold the robot
  still.

  The digital zero-rate level of the gyro can be as high as
  25 degrees per second, and this calibration helps us correct for
  that. */

void turnSensorSetup()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();

  lcd.clear();
  lcd.print(F("Gyro cal"));

  // Turn on the yellow LED in case the LCD is not available.
  ledYellow(1);

  // Delay to give the user time to remove their finger.
  delay(500);

  // Calibrate the gyro.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while (!imu.gyroDataReady()) {}
    imu.readGyro();

    // Add the Z axis reading to the total.
    total += imu.g.z;
  }
  ledYellow(0);
  gyroOffset = total / 1024;

  // Display the angle (in degrees from -180 to 180) until the
  // user presses A.
  lcd.clear();



}


// This should be called to set the starting point for measuring
// a turn.  After calling this, turnAngle will be 0.
void turnSensorReset()
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

// Read the gyro and update the angle.  This should be called as
// frequently as possible while using the gyro to do turns.
void turnSensorUpdate()
{
  // Read the measurements from the gyro.
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;

  // Figure out how much time has passed since the last update (dxt)
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)turnRate * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  turnAngle += (int64_t)d * 14680064 / 17578125;
}
