#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD lcd;
Zumo32U4IMU imu;

//Push kommentar
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
    while(!imu.gyroDataReady()) {}
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



void turn90()
{
  if(turnAngleDegrees>=90&&91>=turnAngleDegrees)
  {
    motors.setSpeeds(0,0);
    rotatCheck=true;
  }
  else
  {
    motors.setSpeeds(-120,100);     
  }
}


void run10()
{
  if(travel_distance<travelMaxDistance)
  {
    motors.setSpeeds(100,100);
  }
  else
  {
    motors.setSpeeds(0,0);
    rotatCheck=false;
  }
}


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
int turnAngle2=turnAngle;
// turnRate is the current angular rate of the gyro, in units of
// 0.07 degrees per second.
int16_t turnRate;

// This is the average reading obtained from the gyro's Z axis
// during calibration.
int16_t gyroOffset;

// This variable helps us keep track of how much time has passed
// between readings of the gyro.
uint16_t gyroLastUpdate = 0;


  //These are the variables for keeping track of how much the encoders have moved.
  /*int16_t countsLeft = encoders.getCountsLeft();
  int16_t countsRight = encoders.getCountsRight();
 */
  //Max distance the robot should drive
  int travelMaxDistance=10;


  //The amount of degrees the robot has turned
  int turnAngleDegrees;


  bool rotatCheck=false;

  //The amount the robot has moved in cm
  int travel_distance;


  int16_t countsLeft = encoders.getCountsLeft();
  int16_t countsRight = encoders.getCountsRight();



    struct Vector
    {
    double x;
    double y;
    };

struct Movement
{
    double dist;
    double angle;
};

struct Movement calculateMove(Vector vec){//calculates and returns a distance and a turn angle
    Movement localMove;
    Vector localVec;
    double gTheta = getAngle()*PI/180; //getAngle() is function to get angle between global x-axis and local (the robots) x-axis. Here converted to radians
    localVec.x =  cos(gTheta)*vec.x + sin(gTheta)*vec.y;
    localVec.y = -sin(gTheta)*vec.y + cos(gTheta)*vec.x;
    localMove.dist = sqrt(pow(localVec.x, 2) + pow(localVec.y, 2));
    localMove.angle = acos( localVec.x / localMove.dist )*180/PI;
    return localMove;
}

Vector globalMove1 = { 3, 3};
Vector globalMove2 = { 1, 2};
Vector globalMove3 = { 1,-3};
Vector globalMove4 = {-5,-1};
Vector globalMove5 = { 0,-1};

Movement move1 = calculateMove(globalMove1);
Movement move2 = calculateMove(globalMove2);
Movement move3 = calculateMove(globalMove3);
Movement move4 = calculateMove(globalMove4);
Movement move5 = calculateMove(globalMove5);

void exeMove(Movement move){//turns and drives according to input dist and angle
    turnAngle(move.angle);//command that rotates certain distance and stops
    driveDistance(move.dist);//Command that drives certain distance and stops
}







void setup(){
    Serial.begin(9600);
}

void loop() 
{
  countsRight = encoders.getCountsRight();
  countsLeft = encoders.getCountsLeft();
  
  //For the left side
  int left_cm=(countsLeft/900)*3.14*4;

  //For the right side
  int right_cm=(countsLeft/900)*3.14*4;


  //The amount the robot has travlled
  travel_distance=(left_cm+right_cm)/2;  


  turnSensorUpdate();
  turnAngleDegrees=((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.gotoXY(0, 0);
  lcd.print((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.print(F("   "));



  turn90();
  if(rotatCheck==true)
  {
    run10();
    turnSensorReset();
  }    
}