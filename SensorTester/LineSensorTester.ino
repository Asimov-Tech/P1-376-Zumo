#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;

//int searchLeft;
//int searchRight;

#define NUM_SENSORS 5 //Number of activated sensors
uint16_t sensorValues[NUM_SENSORS]; //Some array that contains the raw read values from the sensors between 0-2000
bool useEmitters = true;
struct LineSensorsWhite { //Datatype that stores the boolean values for the sensorStates
    bool left;
    bool leftCenter;
    bool Center;
    bool rightCenter;
    bool right;
};

bool left = false;
bool Center =false;
bool right  =false;

int threshold; // White threshold, white return values lower than this
int threshold2;
int threshold3;
LineSensorsWhite sensorState = {false,false,false,false,false};

void setup() {
    lineSensors.initFiveSensors(); // Initializes five sensors
    readSensors(sensorState);      //fills structure with first bools
    Serial.begin(9600);
    //calibrWht();                   //Calibrates the sensors for white lines
}

void loop() 
{ 
  printSensorReadings();
  lineChecker();
}

void  lineChecker()
{
  if(left ==  false && Center ==  false &&  right ==  false)
  {
    motors.setSpeeds(100,100);
  }
  else
  {
    motors.setSpeeds(0,0);
  }
}

void printSensorReadings()
{
      readSensors(sensorState);    //fill sensordata
    Serial.println("------------- SENSOR DATA -------------");
    Serial.println("Left sensor value:         " + (String)sensorValues[0] + ", state: " + (String)sensorState.left);
    //Serial.println("Left Center sensor value:  " + (String)sensorValues[1] + ", state: " + (String)sensorState.leftCenter);
    Serial.println("Center sensor value:       " + (String)sensorValues[2] + ", state: " + (String)sensorState.Center);
    //Serial.println("Right Center sensor value: " + (String)sensorValues[3] + ", state: " + (String)sensorState.rightCenter);
    Serial.println("RIGHT sensor value:        " + (String)sensorValues[4] + ", state: " + (String)sensorState.right);
    Serial.println("------------- END OF DATA -------------"); //print sensordata
   delay(200);
}


/*
void calibrWht() { //A funtion that is called in the setup to help calibrate sensors for the conditions at hand
    Serial.println("Press A to calibrate WHITE");
    delay(250);
    buttonA.waitForPress();
    threshold3 = (sensorValues[2]+20);
    threshold2 = ((sensorValues[1] + sensorValues[3]) / 2 + 20); //takes the mean value of the three center sensors and adds some margin
    //threshold = ((sensorValues[0]+sensorValues[4])/2 + 20); //takes the mean value of the outer-most sensors and makes a separate threshold for them
    delay(250);
    Serial.println(threshold); //prints threshold once at the beginning of the code
    Serial.println(threshold2);
    buttonA.waitForPress();
}
*/

//The values are harcoded
void readSensors(LineSensorsWhite& state) {  // Next line reads the sensor values and store them in the array lineSensorValues
    lineSensors.read(sensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF); //Retrieves data from sensors
    state = {false,false,false,false,false}; // state of the sensors is ALWAYS set to negative in the structure, so that the if statements below only change the boolean to true when the conditions are met
    if (sensorValues[0] < 300) {
        state.left = true;
        left=true;
    }
    /*if (sensorValues[1] < threshold2) {
        state.leftCenter = true;
    }*/
    if (sensorValues[2] < 150) {
        state.Center = true;
        Center=true;
    }
    /*if (sensorValues[3] < threshold2) {
        state.rightCenter = true;
    }*/
    if (sensorValues[4] < 300) {
        state.right = true;
        right=true;
    }
}
