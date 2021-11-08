#include <Wire.h>
//#include <Zumo32U4.h>



const int numDest =7;

struct Destination{
    char id;
    int x;//x-coordinate
    int y;//y-coordinate
};

struct Route{
    char stops[numDest];
    double dist;
};

Route shortestRoute = {{},1000000000};


Destination spots[numDest] ={
    {'A', 0, 0},
    {'B', 4, 1},
    {'C', 2, 3},
    {'D',-1, 5},
    {'E',-2, 2},
    {'F',-2,-3},
    {'G', 3,-2}
};

char destinationsID[numDest];
void setupDestinations(){
    for(int i=0; i < numDest; i++)
        destinationsID[i]=i; 
}


double costMatrix[numDest][numDest];
void setupCostMatrix(){
    for(int i=0; i < numDest; i++){//Choose row first
        for(int j=0; j < numDest; j++){//choose column second
            costMatrix[i][j]=sqrt(pow(spots[i].x-spots[j].x,2)+pow(spots[i].y-spots[j].y,2)); //calculates the distance as sqrt(x^2+y^2)
        }
    }
}

void printCostMatrix(){
    Serial.println("    A     B     C     D     E     F     G");
    for(int i=0; i < numDest; i++){//Choose row first
        Serial.print((String)spots[i].id + ": ");
        for(int j=0; j < numDest; j++){//choose column second
            if(j != numDest-1){
                Serial.print((String)costMatrix[i][j] + ", ");
            } else {
                Serial.println((String)costMatrix[i][j]);
            }
        }
    }   
}

void routeInit(){
    setupDestinations();
    setupCostMatrix();
    printCostMatrix();
}


void swapDestinations(int i, int j){
    char pH = destinationsID[i];
    destinationsID[i]=destinationsID[j];
    destinationsID[j]=pH;
}




void permuteDestinations(int l, int r){
    if(l==r){
    //if no more permutation options print the permutation
        double dist = 0;
        for(int i = 0; i<r;i++){
            Serial.print(spots[destinationsID[i]].id);
            if(i<r-1) dist += costMatrix[destinationsID[i]][destinationsID[i+1]];
        }
        Serial.print(" - Distance: " + (String)dist);
        if(dist < shortestRoute.dist){
            for(int i = 0; i<r;i++){
                shortestRoute.stops[i] = spots[destinationsID[i]].id;
            }
            shortestRoute.dist = dist;
            Serial.print(" New shortest!");
        }
        Serial.println();
    } else {
        for(int i = l; i < r; i++){
            swapDestinations(l,i);
            //lock first selection (most cases swap)
            permuteDestinations(l+1,r);
            //do again
            swapDestinations(l,i);
            //backtrack so we can start over
        }
    }

}



void setup(){
  delay(200);
    Serial.begin(9600);
    routeInit();
}

void loop(){
    permuteDestinations(0,numDest);
    Serial.print("Shortest route is: ");
    for(int i=0;i<numDest;i++) Serial.print(shortestRoute.stops[i]);
    Serial.println(" with a distance of " + (String)shortestRoute.dist);
    delay(30000);
}