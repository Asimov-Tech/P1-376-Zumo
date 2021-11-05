#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;


struct Vector{
    double x;
    double y;
};

struct Movement{
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

void loop(){

}