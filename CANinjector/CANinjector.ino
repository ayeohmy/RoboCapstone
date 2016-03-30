//CANinjector: send whatever lovely CAN messages you want.
//TODO: cry a little

//drink orders that make sense: 

//stock messages that make sense:

//everything else is just a bool or a char. 


#include <SquirtCanLib.h>

SquirtCanLib scl;

int slavePin = 10;
int interruptPin = 2;
char msg;

void setup() {
  // put your setup code here, to run once:

  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);

  Serial.begin(9600);
  Serial.println("setup done!");
  Serial.println("enter a message type, then a :, then the message char, to send it.");
  Serial.println("example: DRINK_ORDER:100");

}

void loop() {
  // put your main code here, to run repeatedly:

  String messageIn = Serial.readString();
  //Serial.println(messageIn);
  //parse message type
int msgType = parseType(messageIn); 

  //parse message contents
  msg = parseContents(messageIn); 

  //send msg

      scl.sendMsg(msgType, msg);
}

int parseType(String msgIn) {
  //find the ":"
  int colonIdx = msgIn.indexOf(':');

  //get everything before that
  String msgType = msgIn.substring(0, colonIdx);
  
  if (!(msgType.compareTo("DRINK_ORDER"))) //then they match! 
    return SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER;

  if (!(msgType.compareTo("READY_TO_MOVE")))
    return SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE;

  if (!(msgType.compareTo("RUNNING")))
    return SquirtCanLib::CAN_MSG_HDR_RUNNING;

  if (!(msgType.compareTo("UI_HEALTH")))
    return SquirtCanLib::CAN_MSG_HDR_UI_HEALTH;

  if (!(msgType.compareTo("SERVING_STATUS")))
    return SquirtCanLib::CAN_MSG_HDR_SERVING_STATUS;

  if (!(msgType.compareTo("SERVING_HEALTH")))
    return SquirtCanLib::CAN_MSG_HDR_SERVING_HEALTH;

  if (!(msgType.compareTo("STOCK_STATUS")))
    return SquirtCanLib::CAN_MSG_HDR_STOCK_STATUS;
  
    if (!(msgType.compareTo("PREP_STATUS")))
    return SquirtCanLib::CAN_MSG_HDR_PREP_STATUS;
  
  if (!(msgType.compareTo("PREP_HEALTH")))
    return SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH;

    if (!(msgType.compareTo("MOVING")))
    return SquirtCanLib::CAN_MSG_HDR_MOVING;
  
  if (!(msgType.compareTo("AT_ROW")))
    return SquirtCanLib::CAN_MSG_HDR_AT_ROW;
  
  if (!(msgType.compareTo("DRIVE_HEALTH")))
    return SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH;

}

char parseContents(String msgIn){
  //find the ":"
  int colonIdx = msgIn.indexOf(':');

  //get the stuff after that
  String contents = msgIn.substring(colonIdx+1);
  //turn it to an int and then a char
  char contentsChar = (char) (contents.toInt()); 
}

void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}

