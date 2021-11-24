#include <Wire.h>
//#include <Zumo32U4.h>



const int numDest = 18; //amount of destinations. tested up to 18. took around 2 seconds on arduino UNO (~100 times less processing than zumo)
//With 16 spots it took around 0.15 secs if all print functionality and delay was removed.
//Does not alway come with optimal solution
struct Destination{
    char id; //character to identify the destination
    int x;//x-coordinate
    int y;//y-coordinate
};


struct Route{
    int stops[numDest+1];//Ordered array containing the tour stops. +1 so it can return to start
    double dist;
};
/*
void getCommands(int i, int r){
    //If there are no additional destinations
    if(i == r){
        //Prints message that job is completed
        lcd.gotoXY(0,0);
        lcd.print("  Task  ");
        lcd.gotoXY(0,1);
        lcd.print("Complete"):

    } else {
        //Defines local variables. xDist is distance that must be travelled along x-axis
        //yDist 1 is distance that must be traveled first along y-axis (in case multiple commands are needed)
        double xDist, yDist1, yDist2;
        xDist = spots[i].x - spots[i+1].x;

        if(xDist != 0){//If the x-distance is not 0 mutliple yDist are needed
            //This if statement figures out whether the best route is along the upper or lower bound and calculates distances accordingly
            if(upperbound < spots[i].y+spots[i+1].y){
                yDist1 =  upperBound - spots[i].y;
                yDist2 = -upperbound + spots[i+1].y;
            } else {
                yDist1 = -spots[i].y;
                yDist2 =  spots[i+1].y;
            }
        } else {
            //If there xDist is 0 it is only needed to travel along the y-axis once, so only one distance is defined (as non-0)
            yDist1 = spots[i+1].y-spots[i].y;
            yDist2 = 0;
        }

        //The following 3 switch statements figures out how the robot needs to align itself compared to the global coordinate system. 
        //0 is along initial allignment (the way the robot was initially pointed). -90 is 90 degrees clockwise compared to initial allignment. 
        //90 is 90 degrees counterclockwise compared to initial allignment and -180 is opposite the initial allignment.
        //After each switch statement a driving functions executes
        switch(true){
            case (yDist1 > 0): turnTo(-90); break;
            case (yDist1 < 0): turnTo(90); break; 
        }
        driveDist(xDist);

        switch(true){
            case (xDist > 0): turnTo(0); break;
            case (xDist < 0): turnTo(-180); break; 
        }
        driveDist(xDist);

        switch(true){
            case (yDist2 > 0): turnTo(-90); break;
            case (yDist2 < 0): turnTo(90); break; 
        }
        driveDist(yDist2);
        //Reiterates through the command
        getCommands(i+1,r)
    }
}
*/
Route route = {{},0}; //declaration of the used route (gets edited along the way)


//Destinations with coordinates
Destination spots[numDest] ={
    {'O',  0,  0},
    {'A', 82, 36},
    {'B', 59, 25},
    {'C', 29, 69},
    {'D', 30,  5},
    {'E', 70, 98},
    {'F', 14, 93},
    {'G',  6, 19},
    {'H', 14, 48},
    {'I', 87, 12},
    {'J', 16, 68},
    {'K', 69, 39},
    {'L', 96, 69},
    {'M', 15, 60},
    {'N', 30, 91},
    {'P', 95, 77},
    {'Q', 18, 84},
    {'R', 44, 39}
};

//upper bound of field (used for augmented cost matrix)
int upperBound = 10;
double costMatrix[numDest][numDest];
//Sets up a cost matrix with direct routes between points
void setupCostMatrix(char mod)//uses d for direct and i for indirect
{   //Chooses row first
    for(int i=0; i < numDest; i++)
    {   //chooses column second
        for(int j=0; j < numDest; j++)
        {   //calculates the distance as sqrt(x^2+y^2)
            switch(mod){
                case 'd':
                    costMatrix[i][j]=sqrt(pow(spots[i].x-spots[j].x,2)+pow(spots[i].y-spots[j].y,2)); 
                    break;

                case 'i':
                    double dist1 = abs(spots[j].x-spots[i].x)+2*upperBound-(spots[i].y+spots[j].y);
                    double dist2 = abs(spots[j].x-spots[i].x)+spots[i].y+spots[j].y;
                    if (spots[i].x == spots[j].x){
                        costMatrix[i][j] = abs(spots[i].y-spots[j].y);
                        break;
                    } else if (dist1 < dist2){
                        costMatrix[i][j] = dist1;
                    } else {
                        costMatrix[i][j] = dist2;
                    }
            }
            
        }
    }
}
//Prints the cost matrix in the Serial moniter. for debugging 
void printCostMatrix(){
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
void routeInit(char c){
    setupCostMatrix(c);
    printCostMatrix();
}

//Swaps 2 integer variables' values
void swapInt(int &i, int &j){
    int temp = i;
    i=j;
    j=temp;
}

//Checks if certain index-value(from spots array) has already been visited
bool isVisited(int index){
    bool visited = false;
    for(int i = 0; i < numDest; i++){
        if(index == route.stops[i]){visited = true; break;}//breaks if index is found in route.stops
    }
    return visited;
}


//nearest neighbor solution. Used for final solution
void nnSolution(int startIndex){
        int lowestDistIndex; //placeholder variable
        int i = 0; //iterator
        int currentIndex = startIndex; //last inserted index
        route.stops[i] = startIndex; //Actually inserts index
        int remaining = numDest-1; //1 stop has already been added to the list

        while(remaining > 0){
            //An index value (not equal to one that has already been used) is chosen
            for(int i = 0; i < numDest; i++) if (isVisited(i) == false){lowestDistIndex = i; break;} 
            //for loop that compares the distance to all nonvisited stops from current stop
            for(int i = 0; i < numDest; i++){
                if(isVisited(i) == false && costMatrix[currentIndex][i] < costMatrix[currentIndex][lowestDistIndex])
                    lowestDistIndex = i;
            }
            //adds the dist to of just visited stop to total distance
            route.dist += costMatrix[currentIndex][lowestDistIndex];
            //updates last visited index
            currentIndex = lowestDistIndex;
            i++;
            remaining--;
            //the index value is added to list
            route.stops[i] = currentIndex;
            //information text (for debugging - commented out now)
            //Serial.println("Added stop " + (String)spots[currentIndex].id + ", for new total distance " + (String)route.dist);

        }
            //when all stops have been visited, travels to startpoint again
            route.dist += costMatrix[currentIndex][startIndex];
            route.stops[numDest+1] = currentIndex;
}

//Actual combined solution
void TSP(){
    //First generates path with nn-solution
    nnSolution(0);
    //prints tour for debugging
    printDest();

    bool change = true; //bool to control if values have been swapped
    
    //Repeats if a change has happened
    while(change){
        //bool is flipped to avoid infinite loop
        change = false;
        //checks for all edges if total distance is lower if end points of edges are switched
        for (int i = 0; i < numDest-2; i++){
            for (int j = i+2; j < numDest; j++){
                double oDist = costMatrix[route.stops[i]][route.stops[i+1]] + costMatrix[route.stops[j]][route.stops[j+1]];
                double nDist = costMatrix[route.stops[i]][route.stops[j]] + costMatrix[route.stops[i+1]][route.stops[j+1]];
                if(nDist < oDist){//if total distance is lower (expression is equivelant )

                    //Debug text print below
                    //Serial.println("swapping for change of: " + (String)(nDist - oDist));

                    //Subtracts difference (nDist<oDist always)
                    route.dist += nDist - oDist;
                    //switches edges (and therefore inverts the order between start and end points of edges)
                    invertPartialTour(i+1,j);
                    //change has happened so flips bool
                    change = true;
                    //A break might be necessary but no test has revealed that
                }}}
        //printDest(); //for debug purposes
    }
    //print so that we know it is done
    printDest();
    Serial.println("Route complete");
    
}

void invertPartialTour(int from, int to){
    //swaps elements from opposite ends of the partial list until they pass eachother (or hit the same value)
  while(from <= to){
    swapInt(route.stops[from], route.stops[to]);
    //Both index values go closer to middle of partial list
    from++;
    to--;
  }
}


//Self explanatory
void printDest(){
  Serial.print("Route is: ");
  for (int i = 0; i < numDest+1; i++) Serial.print(spots[route.stops[i]].id);
  Serial.println(" with distance : " + (String)route.dist);
}

void setup(){
  Serial.begin(9600);
  delay(500);
  routeInit('d');
  delay(1000);
}
void loop(){
  route = {{},0};
  TSP();
  delay(10000);




}


