
                                     // If it is 1 then the robot is at the buttom, if it is -1 the robot is at the top.
void driveRestOfField()
{
  while(notDriven ==  false)
  {
    if(ydist<halfTheField)
    {
      distDriveY(-ydist);
      notDriven=true;
      topOrBut=1;
      //Serial.println("hva gud");
    }
    else
    {
      distDriveY(theField-ydist);
      notDriven=true;
      topOrBut=-1;
      //Serial.println("hva satan");
      //Serial.println(topOrBut);
    }
  }
}
