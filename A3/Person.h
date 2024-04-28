#ifndef PERSON_H
#define PERSON_H

#include <string>

using namespace std;

class Person{

    public:

        Person(int floor);

        //~~~~~~ Getters & Setters ~~~~~~~~
        void setWeight(int weight);
        int getWeight() const;
        int getFloor() const;
        void setFloor(int current_floor);

    private:
        int weight;
        int current_floor;
        bool is_blocking_door;
};
#endif
