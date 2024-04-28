#include "ElevatorControlSystem.h"
#include <random>



ElevatorControlSystem::ElevatorControlSystem(int numPeople, int numElevators, int numFloors, int elevatorMaxWeight){


    random_device seed; // Will be used to obtain a seed for the random number engine
    mt19937 gen(seed()); // Initialize the random number generator engine
    uniform_int_distribution<> dis(0, numFloors-1); // Initialize the uniform distribution


    this->numPeople         = numPeople;
    this->numElevators      = numElevators;
    this->numFloors         = numFloors;
    this->elevatorMaxWeight = elevatorMaxWeight;

    is_fire = false;
    is_powerOutage = false;

    int randomFloor = 0;

    //create building elevators
    for(int i = 0; i < numElevators; i++){

        // Generate a random number
        randomFloor = dis(gen);

        elevators[i] = new Elevator(randomFloor, numFloors, elevatorMaxWeight);
    }

    //create building floors
    for(int i =0; i < numFloors; i++){
        floors[i] = new Floor(i);
    }

    //create people
    for(int i =0; i < numPeople; i++){

        // Generate a random number
        randomFloor = dis(gen);

        passengers[i] = new Person(randomFloor);
        floors[randomFloor]->addOccupant(passengers[i]);

    }

    randomFloor = dis(gen);
    safeFloor = randomFloor;
}


ElevatorControlSystem::~ElevatorControlSystem(){

    for(int i = 0; i < numElevators; i++){
      delete elevators[i];  
    }
    

    for(int i =0; i < numFloors; i++){
       delete floors[i];
    }

    for(int i =0; i < numPeople; i++){
        delete passengers[i];
    }
}


//~~~~~~~~~~~Elevator Traversal Features~~~~~~~~~~~

void ElevatorControlSystem::allocate_floor_to_elevator(int floor){

    //if the floor is the last floor and the number is postive, this means up was pressed, which you cant do
    if (floor == (numFloors-1) || floor == MAX_ARR){
        return;
    }

    char direction = 'N'; // Initialize direction to 'N' (neutral)

    // Find the direction of the floor
    if(floor < 0){
        // If the floor is negative, make it positive and set direction to 'D' (down)
        floor = floor * -1;
        direction = 'D';
    } else {
        // If the floor is positive, set direction to 'U' (up)
        direction = 'U';
    }

    // Check if the floor is already allocated to any elevator
    for(int i = 0; i < numElevators; i++){
        for(int j = 0; j < elevators[i]->get_dest_list_size(); j++){
            // If elevator's direction matches the desired direction or it's stationary
            if(elevators[i]->getDirection() == direction && elevators[i]->getDirection()  == 'N'){
                // If the floor is already in the destination list, return
                if (elevators[i]->get_dest(j) == floor){
                    return;
                }
            }
         }
    }

    // Iterate through each elevator
    for(int i = 0; i < numElevators; i++){

        // Check if the elevator's direction matches the desired direction and it's going UP
        if (elevators[i]->getDirection() == direction && direction == 'U'){
            // If we're going up and the floor is larger than our first destination, add it
            if(elevators[i]->get_first_floor() < floor){
                elevators[i]->add_dest(floor, direction);
                return;
            }
        } else if (elevators[i]->getDirection() == direction && direction == 'D'){
            // If we're going down and the floor is before the smaller than our first destination, add it
            if(elevators[i]->get_first_floor() > floor){
                elevators[i]->add_dest(floor, direction);
                return;
            }
        } else if (elevators[i]->getDirection() == 'N'){

            // If the elevator is stationary, add the floor to the list
            elevators[i]->add_dest(floor, 'N');
            return;
        }
    }
}


//~~~~~~~~~~~Passenger Features~~~~~~~~~~~

void ElevatorControlSystem::enterElevator(Person * passenger){

    for(int i =0; i < numElevators; i++){
        if(elevators[i]->getFloor() == passenger->getFloor() && elevators[i]->getDoorStatus()){
            elevators[i]->add_passenger(floors[passenger->getFloor()]->removeOccupant(passenger));
            passenger->setFloor(-1); //-1 for floor means we're on the elevator
        }
    }
}

void ElevatorControlSystem::exitElevator(Person * passenger){


    for(int i =0; i < numElevators; i++){
        if(elevators[i]->getDoorStatus()){

            int disembarkFloor = elevators[i]->remove_passenger(passenger);
            if (disembarkFloor != -1) {
                floors[disembarkFloor]->addOccupant(passenger);
                elevators[0]->sendMessage("Passenger exited to flooor: " + to_string(disembarkFloor));
             }

        }
    }
}

void ElevatorControlSystem::blockDoor(Person * passenger){

    int index = elevatorContains(passenger);

    if(index != -1){
        elevators[index]->toggleDoorBlocked();
    }
}

//~~~~~~~~~~~Safety Features~~~~~~~~~~~


void ElevatorControlSystem::overide_destination_list(int floor){

    for(int i =0; i < numElevators; i++){
        while(elevators[i]->get_dest_list_size() > 0){
            elevators[i]->remove_dest();
        }
        //direction doesn't matter since its the only destination
        elevators[i]->add_dest(floor, 'D');
    }
}

void ElevatorControlSystem::disableElevators(){

    for(int i =0; i < numElevators; i++){
        elevators[i]->toggle_disable();
    }
}

void ElevatorControlSystem::trigger_fire(){

    if(!is_fire){

        elevators[0]->sendMessage("Fire occuring, Safe Floor: " + to_string(safeFloor) );
        overide_destination_list(safeFloor);

        //lock elevator features
        disableElevators();
        is_fire = !is_fire;

    }else{

        elevators[0]->sendMessage("Fire ended");
        disableElevators();
        is_fire = !is_fire;
    }
}

void ElevatorControlSystem::trigger_powerOutage(){

   if(!is_powerOutage){
       elevators[0]->sendMessage("Poweroutage occuring, moving to floor 0");
       overide_destination_list(0);

       //lock elevator features
       disableElevators();
       is_powerOutage = !is_powerOutage;

   }else{

       elevators[0]->sendMessage("Poweroutage ended");
       disableElevators();
       is_powerOutage = !is_powerOutage;
   }
}

void ElevatorControlSystem::call_help(Person * passenger){
    int index = elevatorContains(passenger);
    if(index != -1){
        elevators[index]->call_help(index);
    }
}

//~~~~~~~Setters & Getters~~~~~~~

Person* ElevatorControlSystem::getPassenger(int index) const{
    if (0 <= index && index < numPeople){
        return passengers[index];
    }
    return nullptr;
}

Elevator* ElevatorControlSystem::getElevator(int index) const{
    if (0 <= index && index < numElevators){
        return elevators[index];
    }
    return nullptr;
}

Floor* ElevatorControlSystem::getFloor(int index) const{
    if (0 <= index && index < numFloors){
        return floors[index];
    }
    return nullptr;
}


//~~~~~~~~~Extra Functions-------
int ElevatorControlSystem::elevatorContains(Person* passenger){
    for(int i =0; i < numElevators; i++){
        if( elevators[i]->contains(passenger) ) return i;
    }
    return -1;
}

int ElevatorControlSystem::floorContains(Person* passenger){
    for(int i =0; i < numFloors; i++){
        if( floors[i]->contains(passenger) ) return i;
    }
    return -1;
}



void ElevatorControlSystem::endThreads(){
    for(int i =0; i < numElevators; i++){
        elevators[i]->endThread();
    }
}
