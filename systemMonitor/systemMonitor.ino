//systemMonitor: define the system monitor.
//later, turn this into things to be called by the UI loop...
//contact: Gillian Rosen, gtr@andrew.cmu.edu


//#include <CANlib.h>
#include <SquirtCanLib.h>
#include <LiquidCrystal.h>



int spiPins[] = {11, 12, 13, 10}; //[MOSI, MISO, SCK, SS]
INT32U statusMsgs[] = {SquirtCanLib::CAN_MSG_HDR_UI_HEALTH,
                    SquirtCanLib::CAN_MSG_HDR_SERVING_HEALTH,
                    SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH,
                    SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH
                   };
//CAN IDs of the health messages
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //these are the lcd pins.
int button1Pin = 6;
int button2Pin = 7;
int button3Pin = 8;
int button1State = 0;
int button2State = 0;
int button3State = 0;
int sysRunning = 0;
int slavePin = 5;
int interruptPin = 8;
char maxRow = 30;
char msg;
SquirtCanLib scl;

void setup() {
  // put your setup code here, to run once:'
  lcd.begin(16, 2); // it's a 16x2 lcd
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  
  scl.canSetup(slavePin);
  pinMode(interruptPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);
  while (sysRunning == 0) {
    lcd.setCursor(0, 0);
    lcd.print("press any button");
    lcd.setCursor(0, 1);
    lcd.print("to start service");
    button1State = digitalRead(button1Pin);
    button2State = digitalRead(button2Pin);
    button3State = digitalRead(button3Pin);
    if (button1State | (button2State | button3State)) {
      sysRunning = 1;
      msg = 1;
      scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_RUNNING, msg);
    }
  }
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

      //nice drink running code.... 

      
    } else {
      //go into shutdown mode
      while (sysRunning == 1) {
        lcd.setCursor(0, 0);
        lcd.print("press any button");
        lcd.setCursor(0, 1);
        lcd.print("to end service ");
        button1State = digitalRead(button1Pin);
        button2State = digitalRead(button2Pin);
        button3State = digitalRead(button3Pin);
        if (button1State | (button2State | button3State)) {
          sysRunning = 0;
          msg = 0;
          scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_RUNNING, msg);
          scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE, msg);
        }
      }
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
          lcd.setCursor(0, 0);
          lcd.print("error: UI");
        case SquirtCanLib::CAN_MSG_HDR_SERVING_HEALTH:
          lcd.setCursor(0, 0);
          lcd.print("error: DSERV");
        case SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH:
          lcd.setCursor(0, 0);
          lcd.print("error: DPREP");
          lcd.setCursor(0, 1);
          if (currentMsg == 2) {
            lcd.print("       NOCUPS");
          } else {
            lcd.print("       NODRINKS");
          }
        case SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH:
          lcd.setCursor(0, 0);
          lcd.print("error: UI");
          lcd.setCursor(0, 1);
          lcd.print("       STUCK");
        default:
          break;
      }
    }
  }
  return stat;
}

void receivedMsgWrapper() {
  scl.receivedMsg();
}


