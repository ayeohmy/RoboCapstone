//systemMonitor: define the system monitor.
//later, turn this into things to be called by the UI loop...
//contact: Gillian Rosen, gtr@andrew.cmu.edu
//TODO: figure out if we serve or move after being placed in the aisle
//      add the LED instructions to each LCD message
//      (optional) update startup and shutdown messages to show stock with LEDs 

//#include <CANlib.h>
#include <SquirtCanLib.h>
#include <LiquidCrystal.h>
#define STARTUP 1
#define SHUTDOWN 0
#define LEFT 0
#define RIGHT 1

INT32U statusMsgs[] = {SquirtCanLib::CAN_MSG_HDR_UI_HEALTH,
                       SquirtCanLib::CAN_MSG_HDR_SERVING_HEALTH,
                       SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH,
                       SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH
                      };
//CAN IDs of the health messages
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //these are the lcd pins.

int buttonPins[] = {6, 7, 8};
int buttonStates[] = {0, 0, 0};

int led1Pins[] = {13, 14, 15}; //R,G,B
int led2Pins[] = {16, 17, 18}; //R,G,B
int led3Pins[] = {19, 20, 21}; //R,G,B

const int GREEN[] = {0,255,0};
const int RED[] = {255,0,0};
const int WHITE[] = {255,255,255};
const int BLACK[] = {0,0,0};

int sysRunning = 0;
int slavePin = 5;
int interruptPin = 8;
char msg;

char maxRow = 30;
bool side;
int seat;
char drinkOrder = 0xFF;

SquirtCanLib scl;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); // it's a 16x2 lcd
  for (int i = 0; i < 3; i++) {
    pinMode(led1Pins[i], OUTPUT);
    pinMode(led2Pins[i], OUTPUT);
    pinMode(led3Pins[i], OUTPUT);
    pinMode(buttonPins[i], OUTPUT);
  }


  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);
  robotStartupShutdown(STARTUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  int stat = checkStatus();
  Serial.println(stat);

  if (stat == 0) {
    //then you're running normally or normally done
    char currentrow = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW);
    if (currentrow < maxRow) {
      //run like normal!

      //waiting to move

      //waiting for a drink order to be placed

      // waiting for an order to be prepared

      //waiting for a user to take their drink




    } else {
      //you've passed the last seat so time to shut down
      robotStartupShutdown(SHUTDOWN);
    }

  } else {
    //something is wrong
    sysRunning = 0;
    msg = 0;
    scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_RUNNING, msg);
    scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE, msg);
  }
}

int checkStatus() {
  //do we want any interrupt anything here, or can we slowly shut down?
  //check all yr mailboxes for disasters
  int stat = 0;
  for (int i = 0; i < 4; i++) {
    char currentMsg = scl.getMsg(statusMsgs[i]);
    if (currentMsg != 0) {
      //something is wrong. cry and shut everything off
      
      switch (statusMsgs[i]) {
        case (SquirtCanLib::CAN_MSG_HDR_UI_HEALTH):
          panelDisplay("error: UI","       ",RED,RED,RED);
          break;
        case SquirtCanLib::CAN_MSG_HDR_SERVING_HEALTH:
          panelDisplay("error: DSERV","       ",RED,RED,RED);
          break;
        case SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH:
          
          if (currentMsg == 2) {
            panelDisplay("error: DPREP","       NOCUPS",RED,RED,RED);
          } else {
            panelDisplay("error: DPREP","       NODRINKS",RED,RED,RED);
          }
          
          break;
        case SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH:
          panelDisplay("error: DRIVE","       STUCK",RED,RED,RED);
          break;
        default:
          break;
      }
    }
  }
  return stat;
}

void robotStartupShutdown(int mode) {
  String line2 = "";
  if (mode) {
    //startup
    line2 = "to start service";
  } else {
    //shutdown
    line2 = "to end service ";
  }
  while (sysRunning == !mode) {
    panelDisplay("press any button","to end service",GREEN,GREEN,GREEN);
    for (int i = 0; i < 3; i++) {
      buttonStates[i] = digitalRead(buttonPins[i]);
    }
    if (buttonStates[0] | (buttonStates[1] | buttonStates[2])) {
      sysRunning = mode;
      msg = mode;
      scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_RUNNING, msg);
      scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE, 0);
      //starts serving, not moving
    }
  }

}

void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this wat because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}

void panelDisplay(String line1, String line2,
                  const int led1[], const int led2[], const int led3[]) {
  //displays the given two lines and three LED colors on the UI panel
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  for (int i = 0; i < 3; i++) {
    analogWrite(led1Pins[i], led1[i]);
    analogWrite(led2Pins[i], led1[i]);
    analogWrite(led3Pins[i], led1[i]);
  }
}


