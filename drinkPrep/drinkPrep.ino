//drinkprep.ino: define the drink prep.
//contact: Gillian Rosen, gtr@andrew.cmu.edu

//TODO:
//!!!!!!!add stuff for the stack limit switch pins!!!!!!!
//add code for controlling the compressor
//add code for the pressure sensor
 * 
 * NEWLY ADDED STUFF
 * - compressor control, with the pressure sensor
 * - linear actuator control, with two limit switches
 * - lead screw/stepper control, with two limit switches
 * - 
 * 
 * STUFF WE THREW OUT
 * - stock sensors. we need to count stuff instead.
 * 
 */


#include <SquirtCanLib.h>
#include "DRV8825.h"
enum States {
  WAITING,
  PREPARING
};

//CURRENT ACTUAL PINS: 
/*
 * 0: nothing
 * 1: nothing
 * 2: INT on CAN chip (interruptPin?)
 * 3: nothing
 * 4: nothing
 * 5: valve 2
 * 6: valve 1
 * 7: valve 3
 * 8: pressure sensor (?)
 * 9:  compressor + (?)
 * 10: CS on CAN chip (slavePin?)
 * 11: SI on CAN chip (data input?)
 * 12: SO on CAN chip  (data output?)
 * 13: SCK on CAN chip (clock?)
 * 
 */

//HIGH on the pressure sensor means that the pressure is LOW. 
//so if it's HIGH/1, then you need to turn it on! 

int slavePin = 5;
int interruptPin = 8;
int stockPins[] = {0,1,3,4}; //last one is for cups
int valvePins[] = {6, 5, 7};
int grabberMotorPins[] = {2, 10}; //i THINK we need two pins, one pwr one gnd
int grabberLimitSwitchPins[] = {11, 12}; //pins for grabber limit switches {in, out}
int stackLimitSwitchPins[] = {7,8}; //pins for stack limit switches {bottom,top}
int stackMotorPins[] = {1,13}; //{direction, step} pins
int pressureSensorPin = 8; 
int compressorPin = 9; 
int shutoffPin = 0;

char msg;
States state = WAITING;
char prevDrinkOrder = 0;
bool stockStatus[] = {1, 1, 1, 1};

SquirtCanLib scl;
DRV8825 stepper(200, 1, 13); //steps/rev, dir, step

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 3; i++) {
    pinMode(stockPins[i], INPUT);
    pinMode(valvePins[i], OUTPUT);
  }
  pinMode(grabberMotorPins[0], OUTPUT);
  pinMode(grabberMotorPins[1], OUTPUT);
  pinMode(grabberLimitSwitchPins[0], INPUT);
  pinMode(grabberLimitSwitchPins[1], INPUT);
  pinMode(stackLimitSwitchPins[0], INPUT);
  pinMode(stackLimitSwitchPins[1], INPUT);

  //tell the linear actuator to not go yet
  digitalWrite(grabberMotorPins[0], LOW);
  digitalWrite(grabberMotorPins[1], LOW);


  //set up stepper motor
  stepper.setRPM(1); //1 RPM i guess?   
  stepper.setMicrostep(1); // nope on microstepping

  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);
  msg = 0;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_STOCK_STATUS, char(0xFF));
Serial.begin(9600);
  Serial.println("drinkPrep setup done."); 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //check if it's time to make a drink
  char drinkOrder = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER);
  if (drinkOrder != 0xFF & prevDrinkOrder == 0xFF) {
    //if we just got an order, time to start preparing a drink!
    state = PREPARING;
  }
   
  prevDrinkOrder = drinkOrder;
  if (state == PREPARING) {
    Serial.println("state: PREPARING");
    //prepping:
    //JUST! DO IT!
    msg = 1;
    scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS, msg);
    getCup();

    //parse order
    int whichDrink = parseOrder(drinkOrder);

    //dispense based on order
    //loop and listen to the shutoff sensor
    //while you write HIGH to the solenoid sensor
    bool keepDispensing = true;
    while (keepDispensing) {
      int level = digitalRead(shutoffPin);
      if (level > 4) { // some arbitrary distance
        digitalWrite(valvePins[whichDrink], HIGH);
      } else {
        digitalWrite(valvePins[whichDrink], LOW);
        keepDispensing = false;
      }
      delay(10);
    }
    //ok done! go back to waiting
    state = WAITING;
    msg = 0;
    scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS, msg);

  } else {
    //waiting:
    //check each drink sensor
    for (int i = 0; i < 4; i++) {
      int level = digitalRead(stockPins[i]);
      bool someLevel = bool(level);
      stockStatus[i] = someLevel;
      msg = packStock(stockStatus);
      scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_STOCK_STATUS, msg);

      if ((stockStatus[0] == false && stockStatus [1] == false)
          && stockStatus[2] == false) {
        //you're out of drinks! :(
        msg = 3;
        scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH, msg);
      }

      if (stockStatus[3] == false ) {
        //you're out of cups! :(
        msg = 2;
        scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH, msg);
      }
    } //end for-loop, checking stock

  } //end if-statement, waiting vs. preparing

} //end loop() function

int parseOrder(char drinkOrder) {
  //parses the drink order, returning an int corresponding
  //to the ordered drink.

  //IDEA: which drink you want is just an unsigned char number.
  // leading bit is then added on top; it denotes the side.

  //remove the 'side' bit to get to the good content
  drinkOrder = drinkOrder & 0x7F;  // 0111 1111

  //note: 0x7F will be what you get if you
  //try to parse a 'null' drink order
  return drinkOrder;
}

int getCup() {
  //gets a cup from the stack.
  //returns 0 if everything's OK.

  //stack actuate up
  stepper.rotate(360); //rotate 360 degrees. figure out empirically
                    //how much rotation we actually need per action.
                    
  //reach out
  int outSwitch = digitalRead(grabberLimitSwitchPins[0]);
  int inSwitch = digitalRead(grabberLimitSwitchPins[1]);
  while (!outSwitch) {
    digitalWrite(grabberMotorPins[0], HIGH);
    digitalWrite(grabberMotorPins[1], LOW);

    outSwitch = digitalRead(grabberLimitSwitchPins[0]);
  }
       //stop movin
  digitalWrite(grabberMotorPins[0], LOW);
  digitalWrite(grabberMotorPins[1], LOW);

  //stack actuate down

 stepper.rotate(-300); //rotate -300 degrees. figure out empirically
                    //how much rotation we actually need per action.
                    

  //reach in
  while (!inSwitch) {
    digitalWrite(grabberMotorPins[0], LOW);
    digitalWrite(grabberMotorPins[1], HIGH);

    inSwitch = digitalRead(grabberLimitSwitchPins[1]);
  }

  return 0;
}

char packStock(bool stock[]) {
  char ret = 0x00;
  if (stock[0] == true) {
    ret = ret | 0x80; //1000 0000
  } //else it's already at zeros
  if (stock[1] == true) {
    ret = ret | 0x40; //0100 0000
  } //else it's already at zeros
  if (stock[2] == true) {
    ret = ret | 0x20; //0010 0000
  } //else it's already at zeros
  if (stock[3] == true) {
    ret = ret | 0x10; //0001 0000
  } //else it's already at zeros

  return ret;
}

void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}


