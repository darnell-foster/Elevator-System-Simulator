#include "Person.h"

    
Person::Person(int floor){
        weight        = 100;
        current_floor = floor;
        is_blocking_door    = false;
}



//~~~~~~ Getters & Setters ~~~~~~~~
void Person::setWeight(int weight){
    this->weight = weight;
}

int Person::getWeight() const{
    return weight;
}

void Person::setFloor(int current_floor) {
    this->current_floor = current_floor;
}

int Person::getFloor() const{
    return current_floor;
}

