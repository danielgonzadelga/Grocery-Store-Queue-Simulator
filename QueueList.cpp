#include "QueueList.h"

#include "Customer.h"

QueueList::QueueList() { head = nullptr; }

QueueList::QueueList(Customer* customer) { head = customer; }

QueueList::~QueueList() {
  delete head;
}

Customer* QueueList::get_head() { 
  return head;
 }

void QueueList::enqueue(Customer* customer) {
  if(head==NULL)head=customer;
  else{
    Customer* temp = head;
    while(temp->get_next()!=NULL)temp=temp->get_next();
    temp->set_next(customer);
  }
}

Customer* QueueList::dequeue() {
  if(head == NULL)return NULL;
  Customer* First = head;
  head=head->get_next();
  First->set_next(NULL);
  return First;
}

int QueueList::get_items() {
  int sum=0;
  Customer* temp=head;
  while(temp!=NULL){
    sum+=temp->get_numOfItems();
    temp=temp->get_next();
  }
  return sum;
}

bool QueueList::empty_queue() {
  if(head == NULL)return false;
  delete head;
  head = NULL;
  return true;
  // if the queue is empty, return false
  // if the queue is not empty, delete all customers
  // and return true
  
}

void QueueList::print() {
  // print customers in a queue
  Customer* temp = head;
  while (temp != nullptr) {
    temp->print();
    temp = temp->get_next();
  }
}
