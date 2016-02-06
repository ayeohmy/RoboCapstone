//drinkprep.ino: define the drink prep.
//contact: Gillian Rosen, gtr@andrew.cmu.edu

//TODO: think about indexing (see parseOrder) and
//figure out how you want the messages to look.
//WRITE IT THE HECK DOWN.
//finish getCup() when you know what the linear 
//actuators actually take.

//#include <CANlib.h>
#include <squirtlib.h>
#include <LiquidCrystal.h>


int spiPins[] = {11, 12, 13, 10}; //[MOSI, MISO, SCK, SS]
int stockPins[] = {7, 8, 9, 6}; //last one is for cups
int valvePins[] = {3, 4, 5};
int armMotorPin = 2;
int leadScrewPin = 1;
int shutoffPin = 0;
//omg we're using every single pin...

char msg;
bool prepStatus = 0;
char prevDrinkOrder = 0;
bool stockStatus[] = {1, 1, 1, 1};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 3; i++) {
    pinMode(stockPins[i], INPUT);
    pinMode(valvePins[i], OUTPUT);
  }
  pinMode(armMotorPin, OUTPUT);
  pinMode(leadScrewPin, OUTPUT);

  canSetup(spiPins);
  msg = 0;
  sendMsg(CAN_MSG_HDR_PREP_STATUS, msg);
  sendMsg(CAN_MSG_HDR_PREP_HEALTH, msg);
  sendMsg(CAN_MSG_HDR_STOCK_STATUS, char(0xFF));
}

void loop() {
  // put your main code here, to run repeatedly:
  //check if it's time to make a drink
  char drinkOrder = getMsg(CAN_MSG_HDR_DRINK_ORDER);
  if (drinkOrder != 0xFF & prevDrinkOrder == 0xFF) {
    prepStatus = 1;
  } else {
    //prepStatus = 0;
  }
  prevDrinkOrder = drinkOrder;
  if (prepStatus) {

    //prepping:
    //JUST! DO IT!
    msg = 1;
    sendMsg(CAN_MSG_HDR_PREP_STATUS, msg);
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
    //ok done!
    prepStatus = 0;
    msg = 0;
    sendMsg(CAN_MSG_HDR_PREP_STATUS, msg);

  } else {
    //waiting:
    //check each drink sensor
    for (int i = 0; i < 4; i++) {
      int level = digitalRead(stockPins[i]);
      bool someLevel = bool(level);
      stockStatus[i] = someLevel;
      msg = packStock(stockStatus);
      sendMsg(CAN_MSG_HDR_STOCK_STATUS, msg);

      if ((stockStatus[0] == false && stockStatus [1] == false)
          && stockStatus[2] == false) {
        //you're out of drinks! :(
        msg = 3;
        sendMsg(CAN_MSG_HDR_PREP_HEALTH, msg);
      }

      if (stockStatus[3] == false ) {
      //you're out of cups! :(
        msg = 2;
        sendMsg(CAN_MSG_HDR_PREP_HEALTH, msg);
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

  //cup grab

  //stack actuate down
 
  return 0;
}

char packStock(bool stock[]) {
  char ret = 0x00;
  if (stock[0] == true){
  ret = ret | 0x80; //1000 0000
  } //else it's already at zeros 
    if (stock[1] == true){
  ret = ret | 0x40; //0100 0000
  } //else it's already at zeros 
      if (stock[2] == true){
  ret = ret | 0x20; //0100 0000
  } //else it's already at zeros 
      if (stock[3] == true){
  ret = ret | 0x10; //0100 0000
  } //else it's already at zeros 
  
  return ret;
}



