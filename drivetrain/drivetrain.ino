//drivetrain.ino: code for drivetrain. 
//contact: Gillian Rosen, gtr@andrew.cmu.edu

//TODO: write everything. 

#include <SquirtCanLib.h>
#define STATIONARY 0
#define PREPTOMOVE 1 
#define MOVE 2
#define SIDECLOSE 3
#define STUCK 4

int slavePin = 5;
int interruptPin = 8;
int ultrasoundPins[5][2] = {{1,2},{3,4},{5,6},{7,8},{9,10}}; 
//{{trig1, echo1}, {trig2, echo2}, etc.}
int encoderPins[4][2] = {{11,12},{13,14},{15,16},{17,18}}; 
//{{A1,B1},{A2,B2}, etc.}
int motorPins[] = {19,20,21,22};  

int state = STATIONARY; 
char origRow = 0; 
char msg;

SquirtCanLib scl; 

void setup() {
  // put your setup code here, to run once:

  //navigation sensor setup 
  for(int i = 0; i < 5; i++){
      pinMode(ultrasoundPins[i][0], OUTPUT);
  pinMode(ultrasoundPins[i][1], INPUT);
  }
  //motor stuff setup
for(int i = 0; i < 4; i++){
    pinMode(motorPins[i],OUTPUT); 
    pinMode(encoderPins[i][0],INPUT); 
    pinMode(encoderPins[i][1],INPUT); 
  }
  
  scl.canSetup(slavePin); //pass in slave select pin 
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);
  msg = 0;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_MOVING, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW, msg);  
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH, msg);
}

void loop() {
  // put your main code here, to run repeatedly:

  switch (state){
    case STATIONARY:
    {
      break;
    }
    case PREPTOMOVE:
    {
      break;
    }
    case MOVE:
    {
      break;
    }
    case SIDECLOSE:
    {
      break;
    }
    case STUCK:
    {
      break;
    }
    default: 
    break;
  }//end state switch-statement

}

void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}


