//drinkServing.ino: code for drink serving. 
//contact: Gillian Rosen, gtr@andrew.cmu.edu

//TODO: write drivetrain first. that matters a LOT more.
// then, write this quick little subsystem. 

#include <SquirtCanLib.h>


int slavePin = 5;
int interruptPin = 8;
int la1Pins = {10,11};
int la1Pins = {12,13}; 
SquirtCanLib scl; 


void setup() {
  // put your setup code here, to run once:


  scl.canSetup(slavePin); //pass in slave select pin 
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);
  msg = 0;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_SERVING_STATUS, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_SERVING_HEALTH, msg);
}

void loop() {
  // put your main code here, to run repeatedly:

}


void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}


