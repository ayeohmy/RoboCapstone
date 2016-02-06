//systemMonitor: define the system monitor.
//later, turn this into things to be called by the UI loop...
//contact: Gillian Rosen, gtr@andrew.cmu.edu


//TODO: #define a bunch of stuff in a .h file and import the .h file
//      things to define: error codes, CAN msg headers
//#include <CANlib.h>
#include <squirtlib.h>
#include <LiquidCrystal.h>



int spipins[] = {11, 12, 13, 10}; //[MOSI, MISO, SCK, SS]
int statusMsgs[] = {CAN_MSG_HDR_UI_HEALTH,
                    CAN_MSG_HDR_SERVING_HEALTH,
                    CAN_MSG_HDR_PREP_HEALTH,
                    CAN_MSG_HDR_DRIVE_HEALTH
                   };
//CAN IDs of the health messages
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //these are the lcd pins.
int button1pin = 6;
int button2pin = 7;
int button3pin = 8;
int button1state = 0;
int button2state = 0;
int button3state = 0;
int sysrunning = 0;
char maxrow = 30;
char msg;

void setup() {
  // put your setup code here, to run once:'
  lcd.begin(16, 2); // it's a 16x2 lcd
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);
  pinMode(button3pin, INPUT);

  canSetup(spipins);
  while (sysrunning == 0) {
    lcd.setCursor(0, 0);
    lcd.print("press any button");
    lcd.setCursor(0, 1);
    lcd.print("to start service");
    button1state = digitalRead(button1pin);
    button2state = digitalRead(button2pin);
    button3state = digitalRead(button3pin);
    if (button1state | (button2state | button3state)) {
      sysrunning = 1;
      msg = 1;
      sendMsg(CAN_MSG_HDR_RUNNING, msg);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int stat = checkStatus();
  Serial.println(stat);

  if (stat == 0) {
    //then you're running normally or normally done
    char currentrow = getMsg(CAN_MSG_HDR_AT_ROW);
    if (currentrow < maxrow) {
      //run like normal!

      //nice drink running code.... 

      
    } else {
      //go into shutdown mode
      while (sysrunning == 1) {
        lcd.setCursor(0, 0);
        lcd.print("press any button");
        lcd.setCursor(0, 1);
        lcd.print("to end service ");
        button1state = digitalRead(button1pin);
        button2state = digitalRead(button2pin);
        button3state = digitalRead(button3pin);
        if (button1state | (button2state | button3state)) {
          sysrunning = 0;
          msg = 0;
          sendMsg(CAN_MSG_HDR_RUNNING, msg);
          sendMsg(CAN_MSG_HDR_READY_TO_MOVE, msg);
        }
      }
    }

  } else {
    //something is wrong
    sysrunning = 0;
    msg = 0;
    sendMsg(CAN_MSG_HDR_RUNNING, msg);
    sendMsg(CAN_MSG_HDR_READY_TO_MOVE, msg);
  }
}

void canSetup(int spipins[]) {
  //dummy function for CAN setup
  Serial.print("doing CAN network setup with pins ");
  Serial.print(spipins[0]);
  Serial.print(", ");
  Serial.print(spipins[1]);
  Serial.print(", ");
  Serial.print(spipins[2]);
  Serial.print(", ");
  Serial.println(spipins[3]);

}
int sendMsg(int msgHdr, char payload) {
  //dummy function for CAN library sendMsg
  return 0;

}
char getMsg(int msgHdr) {
  //dummy function for CAN library getMsg
  return '0';
}

int checkStatus() {
  //do we want any interrupt anything here, or can we slowly shut down?
  //check all yr mailboxes for disasters
  int stat = 0;
  for (int i = 0; i < 4; i++) {
    char currentMsg = getMsg(statusMsgs[i]);
    if (currentMsg != 0) {
      //something is wrong. cry and shut everything off
      switch (statusMsgs[i]) {
        case (CAN_MSG_HDR_UI_HEALTH):
          lcd.setCursor(0, 0);
          lcd.print("error: UI");
        case CAN_MSG_HDR_SERVING_HEALTH:
          lcd.setCursor(0, 0);
          lcd.print("error: DSERV");
        case CAN_MSG_HDR_PREP_HEALTH:
          lcd.setCursor(0, 0);
          lcd.print("error: DPREP");
          lcd.setCursor(0, 1);
          if (currentMsg == 2) {
            lcd.print("       NOCUPS");
          } else {
            lcd.print("       NODRINKS");
          }
        case CAN_MSG_HDR_DRIVE_HEALTH:
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


