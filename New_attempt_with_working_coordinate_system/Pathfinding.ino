int i = 0;
int maxY = 10;
int x2,y2;
int wildOats[2][7] = {{1,2,3,4,5,6,7},{2,5,7,9,4,6,3}};
int currentPose[2] = {x2,y2};
int moveThis[2] = {0,0};
int returnToX[2] = {currentPose[0],0}; //Return to x-axis
int returnToY[2] = {currentPose[0], maxY}; //Return to the the maximum y-value on the field

void whereTo(){
  moveThis[0] = wildOats[0][i] - currentPose[0];
  moveThis[1] = wildOats[1][i] - currentPose[1];
  currentPose[0] = wildOats[0][i];
  currentPose[1] = wildOats[1][i];
}

void returning(){
  if (currentPose[1] < maxY/2){
    //drive to returnToX[1] on the y-axis
  }
  else{
   // drive to returnToY[1]
  }
}


void pathFinding() {
  // put your main code here, to run repeatedly:
  for (i = 0; i < 7; i++){
    whereTo();
    distDriveX(moveThis[0]);
    checkxdriven();
    delay(2000);
    checkturn90();
    delay(2000);
    Serial.println(moveThis[1]);
    distDriveY(moveThis[1]);
    checkydriven();
    delay(1000);
    
    /*
    checkturn90();
    checkydriven();
    checkturnminus90();
    // kør først x og så y
    returning();
    */
  }

}
