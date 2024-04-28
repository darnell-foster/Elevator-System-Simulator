#include "Floor.h"


Floor::Floor(int floor_number){
    this->floor_number  = floor_number;
    up_pressed          = false;
    down_pressed        = false;
    numPeople           = 0;
}

//if the passenger is on that floor then the press will work
int Floor::call_elevator_up(){

    //shouldn't need if statement, but just in case
    if(numPeople > 0){
        up_pressed = true;
        return floor_number;
    }
    return MAX_ARR;
}

int Floor::call_elevator_down(){

    if(numPeople > 0 && floor_number!= 0){
        down_pressed = true;
        return (-1*floor_number);
    }
    return MAX_ARR;
}


void Floor::addOccupant(Person* passenger) {

    occupants[numPeople++] = passenger;
    passenger->setFloor(floor_number);
}

Person* Floor::removeOccupant(Person * passenger) {

    for(int i =0; i< numPeople; i++){
        if (occupants[i] == passenger){

            numPeople--;
            for(int j =i; j< numPeople; j++){
               occupants[j] = occupants[j+1];
            }
            passenger->setFloor(-1);
            return passenger;
        }
    }
    return nullptr;
}

bool Floor::contains(Person * passenger){
    //if they're already in the elevator return false
    for(int i =0; i < numPeople; i++){
        if (occupants[i] == passenger){
            return true;
        }
    }
    return false;
}

//~~~~~~ Getters & Setters ~~~~~~~~

bool Floor::get_up_pressed() const{
    return up_pressed;
}

bool Floor::get_down_pressed() const{
    return up_pressed;
}

void Floor::toggle_up_pressed(){
    if (up_pressed) up_pressed = !up_pressed;
}

void Floor::toggle_down_pressed(){
    if (down_pressed) down_pressed = !down_pressed;
}

