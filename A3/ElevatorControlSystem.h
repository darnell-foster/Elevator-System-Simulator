#ifndef ELEVATORCONTROLSYSTEM_H
#define ELEVATORCONTROLSYSTEM_H

#include <string>
#include "Elevator.h"
#include "Floor.h"
#include "Person.h"
#include "defs.h"
#include <random>

using namespace std;

class ElevatorControlSystem{

    public:

        ElevatorControlSystem(int numPeople = 3, int numElevators = 3, int numFloors = 7, int elevatorMaxWeight = 200);
        ~ElevatorControlSystem();

        //~~~~~~~~~~~Elevator Traversal Features~~~~~~~~~~~

        //Find a suitable elevator to service a floor request and adds the request to the elevator
        void allocate_floor_to_elevator(int floor);

        //~~~~~~~~~~~Door Features~~~~~~~~~~~
        void enterElevator(Person * passenger);
        void exitElevator(Person * Passenger);
        void blockDoor(Person * passenger);

        //~~~~~~~~~~~Safety Features~~~~~~~~~~~
         void trigger_fire();
         void trigger_powerOutage();
         void overide_destination_list(int floor);
         void disableElevators();
         void call_help(Person * passenger);


        //~~~~~~~Setters & Getters~~~~~~~
        Person* getPassenger(int index) const;
        Elevator* getElevator(int index) const;
        Floor* getFloor(int index) const;
        int elevatorContains(Person* passenger);
        int floorContains(Person* passenger);

        //-----other-----
        void endThreads();


    private:
        //building settings
        bool is_fire;
        bool is_powerOutage;

        // Containers
        Elevator* elevators[MAX_ARR];
        Floor* floors[MAX_ARR];
        Person* passengers[MAX_ARR];

        //simulation settings
        int numPeople;
        int numElevators;
        int numFloors;
        int elevatorMaxWeight;
        int safeFloor;

};
#endif
