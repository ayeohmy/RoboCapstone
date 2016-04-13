//CANinjector: send whatever lovely CAN messages you want.
//TODO: cry a little
//INPUT FORMAT: MSG_TYPE:msg
//drink orders that make sense: 1, 2, 3, 129, 130, 131 (left)

//stock messages that make sense:

//everything else is just a bool or a char. 


#include <SquirtCanLib.h>

SquirtCanLib scl;

int slavePin = 48;
int interruptPin = 49;
char msg;

void setup() {
  // put your setup code here, to run once:

  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);

  Serial.begin(9600);
  Serial.println("setup done!");
  Serial.println("enter a message type, then a :, then the message char, to send it.");
  Serial.println("example: DRINK_ORDER:2");

}

void loop() {
  // put your main code here, to run repeatedly:

  String messageIn = Serial.readString();
  //Serial.println(messageIn);
  //parse message type
int msgType = parseType(messageIn); 
Serial.println(msgType); 
  //parse message contents
  msg = parseContents(messageIn); 
Serial.println((int) msg); 
Serial.println("-----"); 
  //send msg
  if(msgType < 1000){
      scl.sendMsg(msgType, msg);
  }
}

int parseType(String msgIn) {
  //find the ":"
  int colonIdx = msgIn.indexOf(':');

  //get everything before that
  String msgType = msgIn.substring(0, colonIdx);
  
  if (!(msgType.compareTo("DRINK_ORDER"))){//then they match! 
      Serial.println("message type = DRINK ORDER");   
     return SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER;
  }
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
  String contents = msgIn.substring(colonIdx+1,colonIdx+2);
  //turn it to an int and then a char
  Serial.print("toInt:");
  Serial.println(contents.toInt()); 
  return (char) (contents.toInt()); 
}

void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}

