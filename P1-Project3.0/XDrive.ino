void distDriveX(double dist) 
{
  resetEncoders();
  int f = 1;
  if (dist < 0) f = -1;
  {
    count = dist*78.5; //Don't ask, it just works
    //Serial.println((String)count);
    //Serial.println("Encoder1 count = " + (String)EncoderCount);
    while(f*encoders.getCountsRight()< f*count) 
    {   
      motors.setSpeeds(f*111,f*100);
    }
      motors.setSpeeds(0, 0);
      xdriven=true;
  }
}

void distDriveY(double dist) 
{
  resetEncoders();
  int f = 1;
  if (dist < 0) f = -1;
  //Serial.println("Gets here");
  if (topOrBut < 0) f*topOrBut;
  //Serial.println("Gets here2");
    count = dist*78.5; //Don't ask, it just works
    //Serial.println((String)count);
    //Serial.println("Encoder1 count = " + (String)EncoderCount);
    while(f*encoders.getCountsRight()< f*count) 
    {   
      motors.setSpeeds(f*100,f*100);
    }
    motors.setSpeeds(0, 0);
    ydriven=true;
      
}
