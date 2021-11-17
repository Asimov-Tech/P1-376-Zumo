
void driveRestOfField()
{
  while(notDriven ==  false)
  {
    if(xdist<halfTheField)
    {
      distDriveX(-xdist);
      notDriven=true;
    }
    else
    {
      distDriveX(theField-xdist);
      notDriven=true;
    }
  }
}
