void driveRest();
{
  if(xdist<halfTheField)
  {
    distDriveX(-xdist)
  }
  else
  {
    distDriveX(theField-xdist);
  }
}
