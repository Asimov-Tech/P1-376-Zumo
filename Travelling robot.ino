#include <Wire.h>
#include <Zumo32U4.h>


char destinations[] = {'A','B','C','D','E'};


void swapDestinations(int i, int j){
    char pH = destinations[i];
    destinations[i]=destinations[j];
    destinations[j]=pH;
}




void permuteDestinations(int l, int r){
    if(l==r){
    //if no more permutation options print the permutation
        for(int i = 0; i<r;i++) Serial.print((String)destinations[i]);
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
    Serial.begin(9600);
}

void loop(){
    permuteDestinations(0,5);
    delay(10000);  
}