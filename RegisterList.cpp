#include "RegisterList.h"

#include <iostream>

#include "Register.h"
using namespace std;

RegisterList::RegisterList() {
  head = nullptr;
  size = 0;
}

RegisterList::~RegisterList() {
  delete head;
}

Register* RegisterList::get_head() { 
  return head;
}

int RegisterList::get_size() { 
  return size;
}


Register* RegisterList::get_min_items_register() {
  Register* fewest=this->get_free_register();
  if(fewest!=NULL) return fewest;
  fewest=head;
  Register* temp=head->get_next();
  for(int i = 1;i<size;i++){
    if(fewest->get_queue_list()->get_items() > temp->get_queue_list()->get_items())fewest=temp;
    temp=temp->get_next();
  }
  return fewest; 
}

Register* RegisterList::get_free_register() {
  Register* temp=head;
  for(int i = 0;i<size;i++){
    if(temp->get_queue_list()->get_items() == 0)return temp;
    temp=temp->get_next();
  }
  return NULL;

  // return the register with no customers
  // if all registers are occupied, return nullptr
}

void RegisterList::enqueue(Register* newRegister) {
  if(head == NULL)head=newRegister;
  else{
    Register* temp=head;
    for(int i = 1;i<size; i++)temp=temp->get_next();
    temp->set_next(newRegister);
  }
  size++;
  // a register is placed at the end of the queue
  // if the register's list is empty, the register becomes the head
  // Assume the next of the newRegister is set to null
  // You will have to increment size 
  
}

bool RegisterList::foundRegister(int ID) {
  Register* temp=head;
  for(int i = 0;i<size;i++){
    if(temp->get_ID() == ID)return true;
    temp=temp->get_next();
  }
  return false;
  // look for a register with the given ID
  // return true if found, false otherwise

}

Register* RegisterList::dequeue(int ID) {
  Register* temp=head;
  if(temp == NULL)return NULL;

  if(temp->get_ID() == ID){
    size--;
    head = temp->get_next();
    temp->set_next(NULL);
    return temp;
  }


  while(1){
    if(temp->get_next()==NULL)return NULL;
    if(temp->get_next()->get_ID() == ID){
      size--;
      Register* returnR = temp->get_next();
      temp->set_next(returnR->get_next());
      returnR->set_next(NULL);
      return returnR;
    }
    temp=temp->get_next();
  }
  // dequeue the register with given ID

  // return the dequeued register
  // return nullptr if register was not found
}

Register* RegisterList::calculateMinDepartTimeRegister(double expTimeElapsed) {
  Register* temp=head;
  Register* shortest=NULL;
  if(temp == NULL)return NULL;
  bool Empty=true;
  for(int i = 0;i<size;i++){
    if(temp->get_queue_list()!=NULL && temp->get_queue_list()->get_items() != 0){
      if(shortest == NULL)shortest=temp;
      Empty = false;
      if(temp->calculateDepartTime() < shortest->calculateDepartTime())shortest=temp;
    }
    temp=temp->get_next();
  }
  if (Empty)return NULL;
  return shortest;
  // return the register with minimum time of departure of its customer
  // if all registers are free, return nullptr
}

void RegisterList::print() {
  Register* temp = head;
  while (temp != nullptr) {
    temp->print();
    temp = temp->get_next();
  }
}
