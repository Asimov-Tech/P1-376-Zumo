#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4IMU imu;

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
    bool left =false;
    bool Center=false;
    bool right=false;



int threshold; // White threshold, white return values lower than this
LineSensorsWhite sensorState = {false,false,false,false,false};

void setup() {
    lineSensors.initFiveSensors(); // Initializes five sensors
    readSensors(sensorState);      //fills structure with first bools
    Serial.begin(9600);
    //calibrWht();                   //Calibrates the sensors for white lines
    turnSensorSetup();
    delay(500);
    turnSensorReset();
    //motors.setSpeeds(50,50);  
}

bool rotateCheck=false;

void loop() 
{
  printSensor();
  lineChecker();
  
  
}


void lineCheckAndStop()
{
     readSensors(sensorState);    //fill sensordata
     printSensor();
     readSensors(sensorState);
     
    if(left==false && Center == false && right  ==  false) //If sensorState bool on the left sensor is true and if we haven't crossed the first line
    {
      motors.setSpeeds(75 , 75);
        
    }
    else
    {
      motors.setSpeeds(0, 0);               //Begin to drive faster toward second line
      delay(2000);
    }
}

void printSensor()
{
    Serial.println("------------- SENSOR DATA -------------");
    Serial.println("Left sensor value:         " + (String)sensorValues[0] + ", state: " + (String)sensorState.left);
    //Serial.println("Left Center sensor value:  " + (String)sensorValues[1] + ", state: " + (String)sensorState.leftCenter);
    Serial.println("Center sensor value:       " + (String)sensorValues[2] + ", state: " + (String)sensorState.Center);
    //Serial.println("Right Center sensor value: " + (String)sensorValues[3] + ", state: " + (String)sensorState.rightCenter);
    Serial.println("RIGHT sensor value:        " + (String)sensorValues[4] + ", state: " + (String)sensorState.right);
    Serial.println("------------- END OF DATA -------------"); //print sensordata
}


void readSensors(LineSensorsWhite& state) {  // Next line reads the sensor values and store them in the array lineSensorValues
    lineSensors.read(sensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF); //Retrieves data from sensors
    state = {false,false,false,false,false}; // state of the sensors is ALWAYS set to negative in the structure, so that the if statements below only change the boolean to true when the conditions are met
    if (sensorValues[0] < 350) {
        state.left = true;
        left=true;
    }
    /*if (sensorValues[1] < threshold) {
        state.leftCenter = true;
    }*/
    if (sensorValues[2] < 250) {
        state.Center = true;
        Center=true;
    }
    /*if (sensorValues[3] < threshold) {
        state.rightCenter = true;
    }*/
    if (sensorValues[4] < 350) {
        state.right = true;
        right=true;
    }
}


void lineChecker()
{
  if(left=true)
  {
    motors.setSpeeds(0, 75);
  }

  if(Center=true)
  {
    motors.setSpeeds(0, 0);
  }

  if(right=true)
  {
    motors.setSpeeds(75, 0);
  }
  
}
