void testAccuracy() {
delay(1);
  if ((sqrt(sq(wildOats[1][i+1] - poseVector[1]))) < 10) {
    if (sensorsState.L == true && sensorsState.R == true) {
      distToPoint[j] = (sqrt(sq(wildOats[1][i+1] - poseVector[1]))) - 4;
      j++;

       Serial.println((sqrt(sq(wildOats[1][i+1]-poseVector[1]))));
    }
  }
}

void printTestResults() {
  for ( int p = 0; p < 4; ++p ) {
    Serial.println(String(p) + ": " + String(distToPoint[p]));
  }
}
