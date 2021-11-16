#include <Wire.h>
#include <Zumo32U4.h>



const int numDest =7; //amount of destinations
struct Destination{
    char id; //character to identify the destination8
    int x;//x-coordinate
    int y;//y-coordinate
};


struct Route{
    char stops[numDest];//Ordered array containing the array
    double dist;
};

//High number so that all routes are shorter
Route shortestRoute = {{},1000000000};


//Destinations with coordinates
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
//Sets up an array of numbers. Is used in the permutation
void setupDestinations(){
    for(int i=0; i < numDest; i++)
        destinationsID[i]=i; 
}


double costMatrix[numDest][numDest];
//Sets up a cost matrix with direct routes between points
void setupCostMatrix()
{   //Chooses row first
    for(int i=0; i < numDest; i++)
    {   //chooses column second
        for(int j=0; j < numDest; j++)
        {   //calculates the distance as sqrt(x^2+y^2)
            costMatrix[i][j]=sqrt(pow(spots[i].x-spots[j].x,2)+pow(spots[i].y-spots[j].y,2)); 
            }
    }
}
//Prints the cost matrix in the Serial moniter. for debugging 
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

//Uses all setup functions. Important to use the travelling robot code
void routeInit(){
    setupDestinations();
    setupCostMatrix();
    printCostMatrix();
}

//function to swap 2 index positions in the destinationsID array
void swapDestinations(int i, int j){
    char pH = destinationsID[i];
    destinationsID[i]=destinationsID[j];
    destinationsID[j]=pH;
}




void permuteDestinations(int l, int r)
{
    //if no more permutation options print the permutation
    if(l==r)
    {
        //Distance of current route
        double dist = 0;
        //Prints the route
        for(int i = 0; i<r;i++)
        {
            Serial.print(spots[destinationsID[i]].id);
            // checks whether there are any stops left, and if there are adds the distance from current point to next
            if(i<r-1) dist += costMatrix[destinationsID[i]][destinationsID[i+1]];
        }
        //print the distance of the route after the route
        Serial.print(" - Distance: " + (String)dist);
        //Checks whether current route is shorter than the previous shortes route
        if(dist < shortestRoute.dist)
        {
            //changes the point order if it is the case
            for(int i = 0; i<r;i++)
            {
                shortestRoute.stops[i] = spots[destinationsID[i]].id;
            }
            //changes the distance to be the new one
            shortestRoute.dist = dist;
            Serial.print(". New shortest!");
        }
        Serial.println();
    } else {
        for(int i = l; i < r; i++){
            //Swaps first the l'th index with the i'th
            swapDestinations(l,i);
            //reiterates the function but with the just swapped and all previous indexes locked
            permuteDestinations(l+1,r);
            //functionally returns the ID's to the initial position
            swapDestinations(l,i);
            
        }
    }

}


