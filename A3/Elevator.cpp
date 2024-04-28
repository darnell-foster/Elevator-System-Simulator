#include "Elevator.h"


Elevator::Elevator(int current_floor, int max_floors, int max_weight){


    this->current_floor = current_floor;
    this->max_floors    = max_floors;
    this->max_weight    = max_weight;
    cabin_weight        = 0;
    cabin_size          = 0;
    dest_list_size      = 0;
    door_is_blocked     = false;
    door_is_open        = false;
    direction           = 'N';
    continueThread      = false;
    disable             = false;
    door_disabled       = true;
}


//~~~~~~~ ~~~~Travel Features~~~~~~~~~~~

void Elevator::request_floor(int num){

//    sendMessage("current_floor: " + to_string(current_floor) + " num: " + to_string(num));

    //if the requested floor is the same as the current floor, just add it to the list
    if(num == current_floor){
        add_dest(num, direction);
    }

    if(direction == 'U'){
        if(num > current_floor){
            add_dest(num, 'U');
        }

    }else if(direction == 'D'){
        if(num < current_floor){
            add_dest(num, 'D');
        }

    }else if(direction == 'N'){
        if(num > current_floor){
            add_dest(num, 'U');
        }else if(num < current_floor){
            add_dest(num, 'D');
        }
    }

}


int Elevator::travel(){

    // Check if the elevator is moving UP
    if (direction == 'U'){

        // Check if the current floor is one of the floors in the destination list
        if (current_floor == dest_list[0]){

            // If the elevator has reached its destination floor, remove it from the destination list
            remove_dest();

            // If the destination list is empty, change the direction to 'N' (stationary)
            if(dest_list_size == 0)
            {
                direction = 'N';
            }

            return current_floor;
        } else {

            // If the elevator has not reached the destination floor yet
            // Move to the next floor if not at the top; otherwise, change direction to 'D' if the destination list is not empty
            if(current_floor < max_floors-1){
                current_floor++;
//                sendMessage("current_floor: " + to_string(current_floor));

            } else if(dest_list_size > 0){ // Reached the top, but still have items in list
                direction = 'D';
                return current_floor;
            }
        }
    }
    // Check if the elevator is moving DOWN
    else if (direction == 'D'){

        // Check if the current floor is one of the floors is the destination list
        if (current_floor == dest_list[0]){

            remove_dest();

            if(dest_list_size == 0){
                direction = 'N';
            }
            return current_floor;

        } else {

            // If the elevator has not reached the destination floor yet
            // Move to the previous floor if not at the bottom; otherwise, change direction to 'U' if the destination list is not empty
            if(current_floor > 0){
                current_floor--;

            } else if(dest_list_size > 0){
                direction = 'U';
                return current_floor;
            }
        }
    } else { // If the elevator is stationary, give it a direction based on the first destination in the list

        if (current_floor == dest_list[0]){

            remove_dest();

            if(dest_list_size == 0)
            {
                direction = 'N';
            }

            return current_floor;
        }

        // Determine the direction based on the first destination floor
        if(dest_list[0] > current_floor){
            direction ='U'; // Set direction to 'U' if the first destination is above the current floor
        } else if(dest_list[0] < current_floor){
            direction = 'D'; // Set direction to 'D' if the first destination is below the current floor
        }
    }

    return -1; // Return -1 if there's no action taken
}


void Elevator::add_dest(int floor, const char & direction){

    // if the elevator is disabled it wont move
    if(disable) return;


    // Add the floor to the destination list and increment the size
    dest_list[dest_list_size++] = floor;

    // Sort the list in ascending order if the direction is 'U' (up), or descending order if the direction is 'D' (down)
    for (int i = 0; i < dest_list_size - 1; i++) {
        for (int j = 0; j < dest_list_size - i - 1; j++) {
            if (direction == 'U') { // Sort in ascending order
                if (dest_list[j] > dest_list[j + 1]) {
                    // Swap elements if they are out of order
                    int temp = dest_list[j];
                    dest_list[j] = dest_list[j + 1];
                    dest_list[j + 1] = temp;
                }
            } else if (direction == 'D') { // Sort in descending order
                if (dest_list[j] < dest_list[j + 1]) {
                    // Swap elements if they are out of order
                    int temp = dest_list[j];
                    dest_list[j] = dest_list[j + 1];
                    dest_list[j + 1] = temp;
                }
            }
        }
    }
}


void Elevator::remove_dest(){
    // Decrease the size of the destination list by one
    dest_list_size--;

    // Shift elements in the destination list to remove the first element
    for(int i = 0; i < dest_list_size; i++){
        dest_list[i] = dest_list[i+1]; // Move each element one position to the left
    }
}


//~~~~~~~~~~~Door Features~~~~~~~~~~~
void Elevator::open_door(){
    if(!door_is_open && !door_disabled){
        door_is_open = true;
        emit doorOpened();
    }
}

void Elevator::close_door(){
    if(door_is_open && !door_disabled){
        door_is_open = false;
        emit doorOpened();
    }

}

void Elevator::ring(){
    sendMessage("Bell Rings");
}

//~~~~~~~~~~~Safety Features~~~~~~~~~~~


void Elevator::call_help(int index){
    sendMessage("Helped Called from elevator: " + to_string(index));
}


void Elevator::toggle_disable(){
     disable = !disable;
}

//~~~~~~~~~~~Passenger Features~~~~~~~~~~~

bool Elevator::add_passenger(Person * passenger){

    // Check if the elevator door is open and there is still space for passengers
    if (door_is_open && cabin_size < MAX_ARR){
        // Increment cabin_size and add the passenger to the cabin
        cabin[cabin_size++] = passenger;
        // Increase the total weight of passengers in the cabin
        cabin_weight += passenger->getWeight();
        sendMessage("Passenger Entered Cabin, people in cabin: " + to_string(cabin_size));
        return true;
    }
    return false;
}


int Elevator::remove_passenger(Person * passenger){

    // Go through the cabin list and look for the specified passenger to remove
    for(int i = 0; i < cabin_size; i++){

        if (cabin[i] == passenger){

            // Remove the passenger from the cabin by setting the corresponding element to nullptr
            cabin[i] = nullptr;

            // Decrease the cabin size and Shift elements in the cabin list down to fill the gap created by removing the passenger
            cabin_size--;
            for(int j = i; j < cabin_size; j++){
                cabin[j] = cabin[j+1];
            }

            // Update the total weight of passengers in the cabin
            cabin_weight -= passenger->getWeight();
            if(cabin_weight < 0) cabin_weight =0;

            // Return the current floor where the passenger was removed
            return current_floor;
        }

    }

    // If the passenger is not found in the cabin, return -1 to indicate that there was no passenger to remove
    return -1;
}


bool Elevator::contains(Person * passenger){
    //if they're already in the elevator return false
    for(int i =0; i < cabin_size; i++){
        if (cabin[i] == passenger){
            return true;
        }
    }
    return false;
}

//~~~~~~~~~~~Getters & Setters~~~~~~~~~~~

void Elevator::toggleDoorBlocked(){
    if(door_is_open){
        door_is_blocked = !door_is_blocked;

        if(door_is_blocked){
            sendMessage("Blocked Door");
        }else{
            sendMessage("un-blocked Door");
        }

    }
}


int Elevator::getWeight() const{
    return cabin_weight;
}


int Elevator::get_dest_list_size() const{
    return dest_list_size;
}


int Elevator::get_first_floor() const{
    return dest_list[0];
}


void Elevator::set_cabin_weight(int weight){
    max_weight = weight;
}


char Elevator::getDirection() const{
    return direction;
}

int Elevator::get_dest(int index) const{
    if( index < dest_list_size){
        return dest_list[index];
    }else{
        return -1;
    }
}

int Elevator::getFloor() const{
    return current_floor;
}

bool Elevator::cabin_overweight(){
    return (cabin_weight >= max_weight);
}

bool Elevator::getDoorStatus() const{
    return door_is_open;
}

void Elevator::endThread(){
    continueThread = false;
}

//~~~~~~~QThread~~~~~~~

void Elevator::run(){

    continueThread = true; // Set the flag to indicate that the thread should continue running

    // Main thread loop
    while( (dest_list_size > 0 || cabin_size > 0 ) && continueThread){

        // Check if the door is closed and there are destinations in the list
        if(!door_is_open && dest_list_size > 0){

            int reached_floor = travel(); // Move the elevator to the next floor
//            sendMessage("reached_floor: %d", reached_floor);

            if(reached_floor != -1){ // If the elevator reached a floor

                do{

                    if(!continueThread) return; // Exit the run() method if continueThread is false

                    door_disabled = false; // Enable the door
                    ring();
                    sendMessage("Door is opening");
                    open_door();

                    // run this loop again if the door_is_open doesn't change
                    while(door_is_open){
                        for(int i =0; i < 10; i++){
                            sleep(1);

                            if(door_is_open == false) break;
                        }

                        //once the door closes it gives another 2 secconds to change  your mind and  open the door
                        ring();
                        sendMessage("door is closing");
                        close_door();
                        sleep(2);
                    }
                    door_disabled = true; // Disable the door

                    // Check if the cabin is overweight or the door is blocked
                    if(cabin_overweight()){
                       sendMessage("can't close cabin is overweight");
                       sleep(1);
                    } else if(door_is_blocked){
                        sendMessage("can't close doors door is blocked");
                        sleep(1);
                    }

                    if(!continueThread) return; // Exit the run() method if continueThread is false


                }while(cabin_overweight() || door_is_blocked); // Repeat the loop if the cabin is overweight or the door is blocked

            }else{
                sleep(3); // If no floor is reached, wait for some time
            }
            emit changedFloor(); // Emit the finished signal before exiting the run() method

        }

    }

}
