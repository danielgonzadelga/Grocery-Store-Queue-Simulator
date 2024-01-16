#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Customer.h"
#include "QueueList.h"
#include "Register.h"
#include "RegisterList.h"

using namespace std;

// Function Declarations:

// Set mode of the simulation
string getMode();

// Register
void parseRegisterAction(stringstream &lineStream, string mode);
void openRegister(
    stringstream &lineStream,
    string mode);  // register opens (it is upto customers to join)
void closeRegister(stringstream &lineStream, string mode);  // register closes 

// Customer
void addCustomer(stringstream &lineStream,
                 string mode);  // customer wants to join


// Helper functions
bool getInt(stringstream &lineStream, int &iValue);
bool getDouble(stringstream &lineStream, double &dValue);
bool foundMoreArgs(stringstream &lineStream);
void departCustomersUntil(string mode);
void PrintStats();

// Global variables
RegisterList *registerList; // holding the list of registers
QueueList *doneList; // holding the list of customers served
QueueList *singleQueue; // holding customers in a single virtual queue
double expTimeElapsed; // time elapsed since the beginning of the simulation

// List of commands:
// To open a register
// register open <ID> <secPerItem> <setupTime> <timeElapsed>
// To close register
// register close <ID> <timeElapsed>
// To add a customer
// customer <items> <timeElapsed>

int main() {
  registerList = new RegisterList();
  doneList = new QueueList();
  singleQueue = new QueueList();
  expTimeElapsed = 0;

  // Set mode by the user
  string mode = getMode();

  string line;
  string command;

  cout << "> ";  // Prompt for input
  getline(cin, line);
  while (!cin.eof()) {
    stringstream lineStream(line);
    lineStream >> command;
    if (command == "register") {
      parseRegisterAction(lineStream, mode);
    } else if (command == "customer") {
      addCustomer(lineStream, mode);
    } else {
      cout << "Invalid operation" << endl;
    }
    cout << "> ";  // Prompt for input
    getline(cin, line);
  }

  cout<<"Finished at time "<<expTimeElapsed<<endl;
  PrintStats();

  delete registerList;
  delete singleQueue;
  delete doneList;

  // You have to make sure all dynamically allocated memory is freed 
  // before return 0
  return 0;
}

string getMode() {
  string mode;
  cout << "Welcome to ECE 244 Grocery Store Queue Simulation!" << endl;
  cout << "Enter \"single\" if you want to simulate a single queue or "
          "\"multiple\" to "
          "simulate multiple queues: \n> ";
  getline(cin, mode);

  if (mode == "single") {
    cout << "Simulating a single queue ..." << endl;
  } else if (mode == "multiple") {
    cout << "Simulating multiple queues ..." << endl;
  }

  return mode;
}

void addCustomer(stringstream &lineStream, string mode) {
  int items;
  double timeElapsed;
  if (!getInt(lineStream, items) || !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments." << endl;
    return;
  }

  expTimeElapsed+=timeElapsed;
  departCustomersUntil(mode);
  Customer* NC = new Customer(expTimeElapsed, items);
  cout<<"A customer entered"<<endl;


  if(mode == "single"){
    if(registerList->get_free_register() != NULL){
      cout<<"Queued a customer with free register "<<registerList->get_free_register()->get_ID()<<endl;
      registerList->get_free_register()->set_availableTime(expTimeElapsed);
      registerList->get_free_register()->get_queue_list()->enqueue(NC);
    }else{
      cout<<"No free registers"<<endl;
      singleQueue->enqueue(NC);
    }
  }else{
    cout<<"Queued a customer with quickest register "<<registerList->get_min_items_register()->get_ID()<<endl;
    if(registerList->get_min_items_register()->get_queue_list()->get_head()==NULL)
      registerList->get_min_items_register()->set_availableTime(expTimeElapsed);
    registerList->get_min_items_register()->get_queue_list()->enqueue(NC);
  }
    
  
  // Depending on the mode of the simulation (single or multiple),
  // add the customer to the single queue or to the register with
  // fewest items
  
}

void parseRegisterAction(stringstream &lineStream, string mode) {
  string operation;
  lineStream >> operation;
  if (operation == "open") {
    openRegister(lineStream, mode);
  } else if (operation == "close") {
    closeRegister(lineStream, mode);
  } else {
    cout << "Invalid operation" << endl;
  }
}

void openRegister(stringstream &lineStream, string mode) {
  int ID;
  double secPerItem, setupTime, timeElapsed;
  // convert strings to int and double
  if (!getInt(lineStream, ID) || !getDouble(lineStream, secPerItem) ||
      !getDouble(lineStream, setupTime) ||
      !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments" << endl;
    return;
  }

  if(registerList->foundRegister(ID)){
    cout<<"Error: register "<<ID<<" is already open"<<endl;
    return;
  }

  expTimeElapsed+=timeElapsed;
  departCustomersUntil(mode);

  registerList->enqueue(new Register(ID,secPerItem,setupTime,expTimeElapsed));
  cout<<"Opened register "<<ID<<endl;

  if(mode == "single"){
    if(singleQueue->get_head()!=NULL){
      cout<<"Queued a customer with free register "<<registerList->get_free_register()->get_ID()<<endl;
      registerList->get_free_register()->get_queue_list()->enqueue(singleQueue->dequeue());
    }
  }

  // Check if the register is already open
  // If it's open, print an error message
  // Otherwise, open the register
  // If we were simulating a single queue, 
  // and there were customers in line, then 
  // assign a customer to the new register
}

void closeRegister(stringstream &lineStream,string mode) {
  int ID;
  double timeElapsed;
  // convert string to int
  if (!getInt(lineStream, ID) || !getDouble(lineStream, timeElapsed)) {
    cout << "Error: too few arguments." << endl;
    return;
  }
  if (foundMoreArgs(lineStream)) {
    cout << "Error: too many arguments" << endl;
    return;
  }

  expTimeElapsed+=timeElapsed;
  departCustomersUntil(mode);

  if(registerList->foundRegister(ID)){
    delete registerList->dequeue(ID);
    cout<<"Closed register "<<ID<<endl;
  } else cout<<"Error: register "<<ID<<"  is not open"<<endl;
  // Check if the register is open
  // If it is open dequeue it and free it's memory
  // Otherwise, print an error message 
}

bool getInt(stringstream &lineStream, int &iValue) {
  // Reads an int from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  iValue = stoi(command);
  return true;
}

bool getDouble(stringstream &lineStream, double &dvalue) {
  // Reads a double from the command line
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  }
  dvalue = stod(command);
  return true;
}

bool foundMoreArgs(stringstream &lineStream) {
  string command;
  lineStream >> command;
  if (lineStream.fail()) {
    return false;
  } else {
    return true;
  }
}

void departCustomersUntil(string mode){
  if(mode == "single"){
    while(registerList->calculateMinDepartTimeRegister(expTimeElapsed)!=NULL && expTimeElapsed >= registerList->calculateMinDepartTimeRegister(expTimeElapsed)->calculateDepartTime()){
      Register* CurReg= registerList->calculateMinDepartTimeRegister(expTimeElapsed);
      Customer* CurCust= registerList->calculateMinDepartTimeRegister(expTimeElapsed)->get_queue_list()->get_head();


      CurReg->departCustomer(doneList);
      CurReg->get_queue_list()->enqueue(singleQueue->dequeue());

      cout<<"Departed a customer at register ID "<<CurReg->get_ID()<<" at "<<CurCust->get_departureTime()<<endl;
      if(CurReg->get_queue_list()->get_head() != NULL)
        cout<<"Queued a customer with free register "<<CurReg->get_ID()<<endl;
    }
  }else{
    while(registerList->calculateMinDepartTimeRegister(expTimeElapsed)!=NULL && expTimeElapsed >= registerList->calculateMinDepartTimeRegister(expTimeElapsed)->calculateDepartTime()){
      Register* CurReg= registerList->calculateMinDepartTimeRegister(expTimeElapsed);
      Customer* CurCust= registerList->calculateMinDepartTimeRegister(expTimeElapsed)->get_queue_list()->get_head();

      CurReg->departCustomer(doneList);
      cout<<"Departed a customer at register ID "<<CurReg->get_ID()<<" at "<<CurCust->get_departureTime()<<endl;
    }
  }
}

void PrintStats(){
  double NumC=0,max=0,avg=0,stanD=0;
  Customer* CurC=doneList->get_head();

  cout<<"Statistics:"<<endl;
  while(CurC!=NULL){
    NumC++;
    if(CurC->get_departureTime() - CurC->get_arrivalTime() > max)
      max=CurC->get_departureTime() - CurC->get_arrivalTime();

    avg+= CurC->get_departureTime() - CurC->get_arrivalTime();

    CurC=CurC->get_next();
  }

  avg/=NumC;

  CurC=doneList->get_head();
  while(CurC!=NULL){
    stanD+=(pow(((CurC->get_departureTime() - CurC->get_arrivalTime()) - avg),2)/NumC);

    CurC=CurC->get_next();
  }
  stanD=sqrt(stanD);

  cout<<"Maximum wait time: "<<max<<endl;
  cout<<"Average wait time: "<<avg<<endl;
  cout<<"Standard Deviation of wait time: "<<stanD<<endl;
}