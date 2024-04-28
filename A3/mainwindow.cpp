#include "mainwindow.h"
#include "ui_mainwindow.h"


Q_DECLARE_METATYPE(string);


MainWindow::MainWindow(QWidget *parent) //constructor
    : QMainWindow(parent)//this is the QMainWindow constructor
    , ui(new Ui::MainWindow)//This creates the UI class and assigns it the UI member
{
    ui->setupUi(this); //sets up the UI

    //set up Ecs
    ecs = new ElevatorControlSystem(numPassengers, numElevators, numFloors);

    //set up settings
    ui->NumElevators->setValue(numElevators);
    ui->NumFloors->setValue(numFloors);
    ui->NumPassengers->setValue(numPassengers);
    ui->ElevatorsMaxWeight->setValue(maxElevatorWeight);

    //set up our selects
    current_passenger   = -1;
    current_elevator    = -1;

    //set up LCDS
    ui->ElevatorFloor->display("------");
    ui->PassengerFloor->display("-----");

    //set setting buttons
    connect(ui->ResetSimulation, SIGNAL(released()), this, SLOT (resetButtonPressed()));

    //Connects the Floor Buttons to the Floor NumPressed() Function
    connect(ui->Button0, SIGNAL(released()), this, SLOT (floorNumPressed()));
    connect(ui->Button1, SIGNAL(released()), this, SLOT (floorNumPressed()));
    connect(ui->Button2, SIGNAL(released()), this, SLOT (floorNumPressed()));
    connect(ui->Button3, SIGNAL(released()), this, SLOT (floorNumPressed()));
    connect(ui->Button4, SIGNAL(released()), this, SLOT (floorNumPressed()));
    connect(ui->Button5, SIGNAL(released()), this, SLOT (floorNumPressed()));
    connect(ui->Button6, SIGNAL(released()), this, SLOT (floorNumPressed()));

    //Elevator Utillities
    connect(ui->HelpButton,      SIGNAL(released()), this, SLOT (helpButtonPressed()));
    connect(ui->OpenDoorButton,  SIGNAL(released()), this, SLOT (openDoorPressed()));
    connect(ui->CloseDoorButton, SIGNAL(released()), this, SLOT (closeDoorPressed()));


    //Passenger & Elevator Selects
    connect(ui->SelectPassenger, SIGNAL(currentIndexChanged(int)), this, SLOT (passengerSelected()));
    connect(ui->SelectElevator,  SIGNAL(currentIndexChanged(int)), this, SLOT (elevatorSelected()));
    connect(ui->PassengerWeight, SIGNAL(valueChanged(int)), this, SLOT (passengerWeightChanged()));


    //passenger Enter and Exit Buttons
    connect(ui->EnterElevator, SIGNAL(released()), this, SLOT(enterElevatorPressed()));
    connect(ui->ExitElevator,  SIGNAL(released()), this, SLOT(exitElevatorPressed()));
    connect(ui->BlockDoorButton, SIGNAL(released()), this, SLOT(blockDoorPressed()));

    //Elevator Floor Call buttons
    connect(ui->ElevatorUp,   SIGNAL(released()), this, SLOT (callElevatorUp()));
    connect(ui->ElevatorDown, SIGNAL(released()), this, SLOT (callElevatorDown()));


    //Building Controls
    connect(ui->ToggleFire,        SIGNAL(clicked()), this, SLOT (fireToggled()));
    connect(ui->TogglePowerOutage, SIGNAL(clicked()), this, SLOT (powerOutageToggled()));

    qRegisterMetaType<std::string>("std::string");
    //connect elevators so there LCD update on floor change & door light
    for(int i = 0; i < numElevators; i++) {
        connect(ecs->getElevator(i), SIGNAL(changedFloor()), this, SLOT (updateLCD()));
        connect(ecs->getElevator(i), SIGNAL(doorOpened()),  this, SLOT (doorOpenedLight()));
        connect(ecs->getElevator(i), SIGNAL(sendMessage(const string&)), this, SLOT (printToTextBox(const string&)));
    }



}


MainWindow::~MainWindow()
{
    delete ecs;
    delete ui;
}


void MainWindow::resetButtonPressed(){


    printToTextBox("Resting Please Wait...");

    for(int i =0; i < numElevators; i++){
        ecs->endThreads();
        ecs->getElevator(i)->quit();
        ecs->getElevator(i)->wait();
    }


    //updates our values
    numPassengers       = ui->NumPassengers->value();
    numElevators        = ui->NumElevators->value();
    numFloors           = ui->NumElevators->value();
    maxElevatorWeight   = ui->ElevatorsMaxWeight->value();


    //change current selections to none values
    current_passenger   = -1;
    current_elevator    = -1;

    //clear the old contents of the drop down and
    ui->SelectElevator->clear();
    ui->SelectPassenger->clear();

    //create text strings to add the new items to the select drop downs
    QStringList elevatorTexts;
    QStringList passengerTexts;

    elevatorTexts.append("<Elevators>");
    passengerTexts.append("<Passengers>");

    for(int i =0; i < numElevators; i++){
        QString str = "Elevator " + QString::fromStdString(std::to_string(i));
        elevatorTexts.append(str);
    }

    for(int i =0; i < numPassengers; i++){
        QString str = "Passenger " + QString::fromStdString(std::to_string(i));
        passengerTexts.append(str);
    }


    //delete old ecs and create new one
    delete ecs;
    ecs = new ElevatorControlSystem(numPassengers, numElevators, numFloors);

    //update drop downs with update it with the new options
    ui->SelectElevator->addItems(elevatorTexts);
    ui->SelectPassenger->addItems(passengerTexts);

    //set up LCDs
    ui->ElevatorFloor->display("------");
    ui->PassengerFloor->display("------");

    //set up settings
    ui->NumElevators->setValue(numElevators);
    ui->NumFloors->setValue(numFloors);
    ui->NumPassengers->setValue(numPassengers);
    ui->ElevatorsMaxWeight->setValue(maxElevatorWeight);


    //connect elevators so there LCD update on floor change
    for(int i = 0; i < numElevators; i++) {
        connect(ecs->getElevator(i), SIGNAL(changedFloor()), this, SLOT (updateLCD()));
    }


}


void MainWindow::updateLCD(){

    if(current_elevator != -1){
        ui->ElevatorFloor->display(ecs->getElevator(current_elevator)->getFloor());

        if(current_passenger != -1){
            if(ecs->getPassenger(current_passenger)->getFloor() == -1){
               QString str = "E " + QString::fromStdString(std::to_string(current_elevator+1));
               ui->PassengerFloor->display(str);
            }
        }
    }

    for(int i =0; i < numElevators; i++){
        printToTextBox("Elevator: " +  to_string(i) + " current_floor: " + to_string(ecs->getElevator(i)->getFloor()));
    }
}


void MainWindow::floorNumPressed(){


    if(current_passenger != -1){
        //the sender returns a pointer to the buttons that was pressed
        QPushButton *button = (QPushButton *)sender();
        int floor = button->text().toInt();

        if(floor >= numFloors) return;

        printToTextBox("Floor Requested: " + to_string(floor) );

        //need to add a direction
        ecs->getElevator(current_elevator)->request_floor(floor);
    }

}


void MainWindow::updatePassengerInfo(){

    if (current_passenger == -1){
        ui->PassengerWeight->setValue(0);
        ui->PassengerFloor->display("---");

    }else{ // If a passenger is selected

        // Check if the selected passenger is in an elevator and return its index
        int elevator = ecs->elevatorContains(ecs->getPassenger(current_passenger));

        // If the passenger is in an elevator Display elevator index in the PassengerFloor LCD display
        if(elevator != -1){
            QString str = "E " + QString::fromStdString(std::to_string(current_elevator));
            ui->PassengerFloor->display(str);

        }else{ // If the passenger is not in an elevator Display passenger's floor in the PassengerFloor LCD display
            ui->PassengerFloor->display(ecs->getPassenger(current_passenger)->getFloor());
        }

        // Set PassengerWeight LCD display to passenger's weight
        ui->PassengerWeight->setValue(ecs->getPassenger(current_passenger)->getWeight());
    }
}


void MainWindow::passengerSelected(){

    QComboBox *box = (QComboBox *)sender(); // Get the sender ComboBox
    int index = box->currentIndex(); // Get the index of the selected item


    // If the selected item is the default option (index 0)
    if (index <= 0){
        current_passenger = -1;
    }else{ // If a passenger is selected
        current_passenger = index - 1; // Set current_passenger to the index of the selected passenger
        printToTextBox("Passenger : " + to_string(current_passenger) + " selected, Weight: " + to_string(ecs->getPassenger(current_passenger)->getWeight()));
    }
    updatePassengerInfo();

}


void MainWindow::elevatorSelected(){
    QComboBox *box = (QComboBox *)sender();
    int index = box->currentIndex();

    //change current elevator aswell as LCD to match
    if (index <= 0){
        current_elevator = -1;
        ui->ElevatorFloor->display("---");
    }else{
        current_elevator = index-1;
        printToTextBox("Elevator: " + to_string(current_elevator) + " Selected");
        ui->ElevatorFloor->display(ecs->getElevator(current_elevator)->getFloor());
        ui->ElevatorWeight->display(ecs->getElevator(current_elevator)->getWeight());
    }
    doorOpenedLight();


}


void MainWindow::callElevatorUp(){
    printToTextBox("call elevator up");


    //if we have a passenger selected add the floor to an elevator
    if(current_passenger != -1){
        int floor = ecs->floorContains(ecs->getPassenger(current_passenger));

        if(floor == -1) return;

        ecs->allocate_floor_to_elevator(ecs->getFloor(floor)->call_elevator_up());
    }

//    for(int i=0; i < numElevators; i++){
//        printToTextBox("Elevator: " + to_string(i) + ", Destination List Size: " + to_string(ecs->getElevator(i)->get_dest_list_size()));
//    }

    //start threads(make each elevator run)
    for(int i =0; i< numElevators; i++){
        ecs->getElevator(i)->start();
    }

}


void MainWindow::callElevatorDown(){
    printToTextBox("call elevator down");


    //if we have a passenger selected add the floor to an elevator
    if(current_passenger != -1){

        int floor = ecs->floorContains(ecs->getPassenger(current_passenger));

        if(floor == -1) return;

        ecs->allocate_floor_to_elevator(ecs->getFloor(floor)->call_elevator_down());

    }

//    for(int i=0; i < numElevators; i++){
//        printToTextBox("Elevator: " + to_string(i) + ", Destination List Size: " + to_string(ecs->getElevator(i)->get_dest_list_size()));
//    }

    //start threads(make each elevator run)
    for(int i =0; i< numElevators; i++){
        ecs->getElevator(i)->start();
    }
}


void MainWindow::enterElevatorPressed(){
    if(current_passenger != -1){
        ecs->enterElevator(ecs->getPassenger(current_passenger));
        ui->ElevatorWeight->display(ecs->getElevator(current_elevator)->getWeight());
        updatePassengerInfo();
    }
}


void MainWindow::exitElevatorPressed(){

    if(current_passenger != -1){
        ecs->exitElevator(ecs->getPassenger(current_passenger));
        ui->ElevatorWeight->display(ecs->getElevator(current_elevator)->getWeight());
        updatePassengerInfo();
    }
}


void MainWindow::passengerWeightChanged(){


    if(current_passenger != -1){
        ecs->getPassenger(current_passenger)->setWeight(ui->PassengerWeight->value());
        if(current_elevator != -1){
            ui->ElevatorWeight->display(ecs->getElevator(current_elevator)->getWeight());
        }
    }

}


void MainWindow::helpButtonPressed(){
    if(current_passenger != -1){
        ecs->call_help(ecs->getPassenger(current_passenger));
    }
}


void MainWindow::openDoorPressed(){
    if (current_passenger != -1){
        int index = ecs->elevatorContains(ecs->getPassenger(current_passenger));

        if (index != -1){
            ecs->getElevator(index)->open_door();
        }
    }

}


void MainWindow::closeDoorPressed(){
    if (current_passenger != -1){
        int index = ecs->elevatorContains(ecs->getPassenger(current_passenger));

        if (index != -1){
            ecs->getElevator(index)->close_door();
        }
    }
}


void MainWindow::doorOpenedLight(){
    //if no elevator selected turn light red
    if(current_elevator == -1){
        ui->DoorLight->setStyleSheet("background-color: red;");
    }else{

        if(ecs->getElevator(current_elevator)->getDoorStatus()){
            ui->DoorLight->setStyleSheet("background-color: green;");
        }else{
            ui->DoorLight->setStyleSheet("background-color: red;");
        }
    }
}


void MainWindow::fireToggled(){

    ecs->trigger_fire();

    //start threads(make each elevator run)
    for(int i =0; i< numElevators; i++){
        ecs->getElevator(i)->start();
    }
}


void MainWindow::powerOutageToggled(){
   ecs->trigger_powerOutage();
   //start threads(make each elevator run)
   for(int i =0; i< numElevators; i++){
       ecs->getElevator(i)->start();
   }
}


void MainWindow::blockDoorPressed(){
    ecs->blockDoor(ecs->getPassenger(current_passenger));
}


void MainWindow::printToTextBox(const string& text) {

//     ui->Console->clear();

//    // Append text to the QTextEdit widget
//    QString str = QString::fromStdString(text + "\n");
//    ui->Console->append(str);

    const int maxLines = 8; // Maximum number of lines to keep in the QTextEdit

    // Append text to the QTextEdit widget
    QString str = QString::fromStdString(text);
    ui->Console->append(str); // Append the new text

    // Check the number of lines in the QTextEdit
    int lineCount = ui->Console->document()->blockCount();

    // If the number of lines exceeds the maximum, clear the QTextEdit
    if (lineCount > maxLines) {
        ui->Console->clear();
    }

}
