#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QThread>
#include <string>
#include "Person.h"
#include "defs.h"


using namespace std;

class Elevator : public QThread{

    Q_OBJECT

    void run() override;

    signals:
        void changedFloor(); //signal used to inform UI the elevator has changed floors
        void doorOpened();  //signal used to inform UI the elevator has opened/closed its doors
        void sendMessage(const string& message);

    public:

        Elevator(int current_floor, int max_floor, int max_weight);
        
        //~~~~~~~~~~~Travel Features~~~~~~~~~~~
        // Deals with cabin elevator floor request
        void request_floor(int num);
        // Makes the elevator move, and updates it visit list
        int  travel();
        // Adds a floor to the elevators destination list, and sorts order based on direction of elevator
        void add_dest(int floor, const char &direction);
        // Removes the floorn from the elevator destination list
        void remove_dest();


        //~~~~~~~~~~~Door Features~~~~~~~~~~~
        void open_door();
        void close_door();
        void ring();


        //~~~~~~~~~~~Safety Features~~~~~~~~~~~
        void call_help(int index);
        void fire_rotine();
        void overideDestList();
        bool cabin_overweight();



        //~~~~~~~~~~~Passenger Features~~~~~~~~~~~
        //adds Person to elevator cabib
        bool add_passenger(Person * passenger);
        //removes Person to elevator cabin and returns a int refering to the current floor
        int  remove_passenger(Person * passenger); 
        bool contains(Person * passenger);
        void endThread();

        //~~~~~~~~~~~Getters & Setters~~~~~~~~~~~
        void toggle_disable();
        void toggleDoorBlocked();
        int get_dest_list_size() const;
        int get_first_floor() const;
        void set_cabin_weight(int weight);
        int getWeight() const;
        char getDirection() const;
        int get_dest(int index) const;
        int getFloor() const;
        bool getDoorStatus() const;


    private:     

        int current_floor;
        int max_floors;
        int dest_list[MAX_ARR];
        int dest_list_size;
        int cabin_size;
        int cabin_weight;
        int max_weight;
        Person* cabin[MAX_ARR];
        bool door_is_open;
        bool door_is_blocked;
        bool disable;           //used to disable elevator so it can't have more destinations added
        bool continueThread;    //used to reset threads
        bool door_disabled;     //used to disable elevator DOOR so it wont open and close
        char direction;         // u = up, d = down, n = null

};
#endif
