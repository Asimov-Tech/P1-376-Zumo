
void driveRestOfField()
{
  while(notDriven ==  false)
  {
    if(ydist<halfTheField)
    {
      distDriveY(-ydist);
      notDriven=true;
      topOrBut=1;
    }
    else
    {
      distDriveY(theField-ydist);
      notDriven=true;
      topOrBut=-1;
    }
  }
}
