#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QMetaType>
#include <string>
#include "Person.h"
#include "Elevator.h"
#include "Floor.h"
#include "ElevatorControlSystem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{

    Q_OBJECT


    public:
        // Constructor
        MainWindow(QWidget *parent = nullptr);

        // Destructor
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        int current_passenger = -1;
        int current_elevator  = -1;
        int numPassengers   = 3;
        int numElevators    = 3;
        int numFloors       = 7;
        int maxElevatorWeight = 300;
        ElevatorControlSystem *ecs;

        // Function to update passenger information in UI
        void updatePassengerInfo();

    private slots:

        void resetButtonPressed();      // Resets the simulation with new settings
        void elevatorSelected();        // sets current elevator
        void floorNumPressed();         // gets the requested foor for elevator
        void helpButtonPressed();       // Help button pressed
        void closeDoorPressed();        // Close door button pressed
        void openDoorPressed();         // Open door button pressed
        void blockDoorPressed();        // Block door button pressed
        void updateLCD();               // Update LCD when elevator changes floors
        void doorOpenedLight();         // Change door light status
        void passengerSelected();       // sets current Passenger
        void passengerWeightChanged();  // Passenger weight changed
        void enterElevatorPressed();    // Enter elevator button pressed
        void exitElevatorPressed();     // Exit elevator button pressed
        void callElevatorUp();          // Call elevator up button pressed
        void callElevatorDown();        // Call elevator down button pressed
        void fireToggled();             // Toggle fire button pressed
        void powerOutageToggled();      // Toggle power outage button pressed

        // Console
        void printToTextBox(const string&); // Print text to console
};
#endif // MAINWINDOW_H
