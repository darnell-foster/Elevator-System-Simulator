#ifndef FLOOR_H
#define FLOOR_H

#include <string>
#include <vector>
#include "Person.h"
#include "defs.h"

using namespace std;

class Floor{

    public:

        Floor(int floor_number);

        //returns the floor number (negative press down)
        int call_elevator_up();
        int call_elevator_down();

        void addOccupant(Person* passenger);
        bool contains(Person * passenger);
        Person* removeOccupant(Person* passenger);

        //~~~~~~ Getters & Setters ~~~~~~~~
        bool get_up_pressed() const;
        bool get_down_pressed() const;
        void toggle_up_pressed();
        void toggle_down_pressed();


    private:
        int floor_number;
        bool up_pressed;
        bool down_pressed;
        Person* occupants[MAX_ARR];
        int numPeople;

};
#endif
