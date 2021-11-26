#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;


double xdist;
double ydist;


int xCoordinates[]  = { 10,  10, 10, 10, 10, 10};

int yCoordinates[]  = { 20,  60, 40,  80,  70, 10};





int coordinatePlace = 0;


double count = 0;
double EncoderCount = 0;
double theField = 100;
double halfTheField  = theField/2;

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

//These are the booleans values that the dependencies in the program relies on
bool xdriven        = false;
bool ydriven        = false;
bool turnedminus90  = false;
bool turned90       = false;


bool notDriven  = false;


int topOrBut;                     //An integer that symbolises wether the zumo is on the top of the field or the buttom. The value is used to decide wether the robot should move in the negative y-coordinate
                                     // If it is 1 then the robot is at the buttom, if it is -1 the robot is at the top.

void turn2(int angle, int speeds) //The function takes in a angle and a speed
{
  bool finish = false;            //Used to check when the zumo has turned the angle degrees
  int f = 1;
  if(angle<0) f = -1;
  while(finish  !=  true)         //A while loop that runs untill the robot has turned the amount of degrees
  {
    if(turnAngleDegrees = 90)
    {
      motors.setSpeeds(0, 0);
      finish  = true;
    }
    else
    {
      motors.setSpeeds(f*-speeds, f*speeds);        //Changes the rotation dependent on the given angle
      imustart();
    }
  }
}


void setup() 
{
  Serial.begin(9600);
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  lcd.clear();
  delay(500);
}

/*This is the entire main loop, the loop only relies on 5 functions and works as intented
*/
void loop() 
{

  imustart();
  nextCoordinates();
  //Serial.println("This is the x-coordinate " + String(xdist));
  //Serial.println("This is the y-coordinate " + String(ydist));
  checkxdriven();
  checkturn90();
  //Serial.println("gets here");
  checkydriven();
  //Serial.println("gets here2");
  delay(1000);        //Dette delay symboliserer at den opsamler havre.
  driveRestOfField();
  checkturnminus90();




}


//------------Functions---------------------Functions-------------------------Functions-------------------------Functions-------------------------Functions-------------------------Functions-------------------------Functions------------------------


void nextCoordinates()      //This function just increments a number to get the next coordinates.
{
  ydist=yCoordinates[coordinatePlace];
  xdist=xCoordinates[coordinatePlace];
  coordinatePlace++;
}



void checkxdriven()
{
  while(xdriven==false)
  {
    distDriveX(xdist);   
  }
}


//This function checks wether xdriven is true, if it is it turns 90 degrees and sets turned90 to true after execution

void checkturn90()
{
  while(xdriven==true)
  {
    if(turnAngleDegrees >= 90 && 91 >= turnAngleDegrees)
    {
      motors.setSpeeds(0, 0);
      turned90=true;
      xdriven=false;
      imustart();
    }
    else
    {
      motors.setSpeeds(-150, 150);
      imustart();
    }
  }
}


//Same as the checkydriven just for the y coordinate.
void checkydriven()
{
  while(turned90==true)
  {
    //Serial.println("gets here3");
    distDriveY(ydist);
    //Serial.println("gets here4");
    ydriven=true;
    
    turned90=false;
    imustart();
  }
}

//This functions makes the zumo turn minus 90 degrees after the ydriven has been run

void checkturnminus90()
{
  while(ydriven==true)
  {
    if(turnAngleDegrees <= 1 && 0 <= turnAngleDegrees)
    {
      motors.setSpeeds(0, 0);
      ydriven=false;
      xdriven=false;  
      notDriven=false;
    }
    else
    {
      motors.setSpeeds(100, -120);
      imustart();
    }
  }
}




///This is important for the turn sensor and the lcd screen.
void imustart()
{
  turnSensorUpdate();
  turnAngleDegrees = ((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.gotoXY(0, 0);
  lcd.print((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.print(F("   "));
}



//Resets the encoders
void resetEncoders() 
{
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
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
