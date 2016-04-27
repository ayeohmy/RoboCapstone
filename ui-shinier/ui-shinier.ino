//ui-shiner.ino: ayyy shiny ui!

/*TODO:
   deal with the "running" state. right now it's always running
   literally all the status monitoring
*/

//CAN error sad: CAN errors: 1001110
//CAN err sad 2:             1111110
// sad 3:        CAN errors: 1001010


//things to include
#include <SquirtCanLib.h>
#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include <watchdog.h>

#define RIGHT 0
#define LEFT 1

enum States {
  REQUESTING,
  PREPARING,
  SERVING,
  DRIVING,
  DONE
};

enum Health {
  FINE,
  NOTFINE
};


//pins to set
int buttonPinsL[] = {24, 22, 23};
int led1PinsL[] = {28, 29, 30}; //R,G,B
int led2PinsL[] = {31, 32, 33}; //R,G,B
int led3PinsL[] = {34, 35, 36}; //R,G,B
int buttonPinsR[] = {27, 26, 25};
int led1PinsR[] = {37,38,39}; //R,G,B
int led2PinsR[] = {40,41,42}; //R,G,B
int led3PinsR[] = {43,44,45}; //R,G,B
int slavePin = 53;
int interruptPin = 21;

//values we'll need to track
int buttonStates[] = {0, 0, 0};
char msg;
bool sendRunning = false;
char drinkOrder = 0;
char readyToMove = 0;
char nowRunning = 0; //use this one later pal
Health health = FINE;
States state = REQUESTING;
int drinksServed = 0;
long drinkTimestamp = 0;
int currentRow = 0;
int currentSide = RIGHT;
long seatTimestamp = 0; 
bool newSeat = true;

//other constants to set
const int GREEN[] = {0, 255, 0};
const int RED[] = {255, 0, 0};
const int BLUE[] = {0, 0, 255};
const int CYAN[] = {0, 255, 255};
const int MAGENTA[] = {255, 0, 255};
const int YELLOW[] = {255, 255, 0};
const int WHITE[] = {255, 255, 255};
const int TEAL[] = {0, 100, 255};
const int SEAF[] = {0,255,125};
const int BLACK[] = {0, 0, 0};

const String drinks[] = {"SQUIRT ", "  WATER", "  CIDER"};
//each of these must be 7 characters; pad as you want them


//objects to construct
LiquidCrystal lcdL(14, 15, 10, 11, 12, 13); //these are the lcd pins.
//(RS,enable, D4,D5,D6,D7)
LiquidCrystal lcdR(20, 7, 16, 17, 18, 19); //these are the lcd pins.
//(RS,enable, D4,D5,D6,D7)

SquirtCanLib scl;
SimpleTimer timer;


void setup() {
  //set up LCD stuff
  lcdL.begin(16, 2); // it's a 16x2 lcd
  lcdR.begin(16,2);

  watchdog_init();

  //set up LEDs and buttons
  for (int i = 0; i < 3; i++) {
    pinMode(led1PinsL[i], OUTPUT);
    pinMode(led2PinsL[i], OUTPUT);
    pinMode(led3PinsL[i], OUTPUT);
    pinMode(buttonPinsL[i], INPUT);
    pinMode(led1PinsR[i], OUTPUT);
    pinMode(led2PinsR[i], OUTPUT);
    pinMode(led3PinsR[i], OUTPUT);
    pinMode(buttonPinsR[i], INPUT);
  }

  //set up CAN
  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, INPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, LOW);

  //set up timers, one for each message
  timer.setInterval(25, drinkOrderUpdate);
  timer.setInterval(50, readyToMoveUpdate);
  //timer.setInterval(50, runningUpdate);
  //timer.setInterval(100, uiHealthUpdate);

  
  
  Serial.begin(9600);
  Serial.println("ui setup done.");
}

void loop() {
  // put your main code here, to run repeatedly:

  /*** things to always do ****/

 if (drinksServed == 1){
  currentSide = LEFT; 
 }
  if (drinksServed >= 3 && state == REQUESTING) {
    drinksServed = 0;
    currentSide = RIGHT;
    state = DRIVING;
    readyToMove = true;
  }

  if (newSeat){
    seatTimestamp = millis();
    newSeat = false; 
  }
  timer.run();

  wdt_reset();

  /*** things to sometimes do, depending on state ***/

  switch (state) {
    case REQUESTING: {
        Serial.print("requesting... drinks served: ");
        Serial.println(drinksServed);

        msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS);
        Serial.print("prep status:");
        Serial.println((int) msg);
        msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW);
        Serial.print("at row:");
        Serial.println((int) msg);


        //get stock status so we know what to show


        //construct the display message
        String timeleft = String(1+(20000-(millis()-seatTimestamp))/1000);
        
        //drink names

        String line1 = getSeatString(); 
        line1 += "  "; //padding spaces
        String line2 = "";
        line1 += drinks[1];
        line1 += "   ";
        line1 += timeleft;
        line2 += drinks[0];
        line2 += "| ";
        line2 += drinks[2];

        //CHANGE THIS to actually actively update
        panelDisplay(line1, line2,CYAN,CYAN,CYAN); //CYAN, MAGENTA, YELLOW);

     
        for (int i = 0; i < 3; i++) {
           if(currentSide == LEFT){
          buttonStates[i] = digitalRead(buttonPinsL[i]);
           } else{
              buttonStates[i] = digitalRead(buttonPinsR[i]);
           }
        }
        if (!buttonStates[0]) {
          panelDisplay(line1, line2, CYAN, BLACK, BLACK);
          drinkOrder = (char) 1;
          drinkTimestamp = millis();
          state = PREPARING;


        }
        if (!buttonStates[1]) {
          panelDisplay(line1, line2, BLACK, MAGENTA, BLACK);
          drinkOrder = (char) 2;
          drinkTimestamp = millis();
          state = PREPARING;


        }
        if (!buttonStates[2]) {
          panelDisplay(line1, line2, BLACK, BLACK, YELLOW);
          drinkOrder = (char) 3;
          drinkTimestamp = millis();
          state = PREPARING;

        }
        //update the seat time
        if (millis() - seatTimestamp > 20000){
          drinksServed++; 
          newSeat = true; 
        }

        break;
      }
    case PREPARING: {
        panelDisplay("Preparing drink", "please wait...", RED, RED, RED);
        delay(100);
        msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS);
        Serial.print("prep status:");
        Serial.println((int) msg);
        Serial.print("time:");
        Serial.println(millis() - drinkTimestamp);
        if (!msg && ((millis() - drinkTimestamp) > 5000)) {
          //then we're done
         
          drinkOrder = (char) 0;
          drinkTimestamp = millis();
          state = SERVING;
        }
        break;
      }
    case SERVING: {
        panelDisplay("Please take  ", "your drink!", GREEN, GREEN, GREEN);

        Serial.println(millis() - drinkTimestamp);

        if (millis() - drinkTimestamp > 5000) {
          //then we're done
          state = REQUESTING;
           drinksServed++;
           newSeat = true;  
        }
        break;
      }
    case DRIVING: {
        panelDisplay("Driving ", "please wait...", BLACK, BLACK, BLACK);
        msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW);
        Serial.print("at row:");
        Serial.println((int) msg);
        Serial.print("current row:");
        Serial.println(currentRow);
        if (msg != currentRow) {
          //then we're done
          readyToMove = false;
          currentRow = msg;
          state = REQUESTING;
          newSeat = true; 
        }
        break;
      }
    default:
      break;
  }

}



/*

 * * * * * HELPER FUNCTIONS

*/




/******* UI HELPERS ********/

/*panelDisplay(LINE1, LINE2, LED1, LED2, LED3):
   displays given text and LEDs on the appropriate lcd.
*/
void panelDisplay(String line1, String line2, const int led1[], const int led2[], const int led3[]) {
  //displays the given two lines and three LED colors on the UI panel
  // lcdL.clear();
   // lcdR.clear();
  //
  while (line1.length() < 16) {
    line1 += " ";
  }
  while (line2.length() < 16) {
    line2 += " ";
  }
  if (currentSide == LEFT) {
    lcdL.setCursor(0, 0);
    lcdL.print(line1);
    lcdL.setCursor(0, 1);
    lcdL.print(line2);
    for (int i = 0; i < 3; i++) {
      analogWrite(led1PinsL[i], led1[i]);
      analogWrite(led2PinsL[i], led2[i]);
      analogWrite(led3PinsL[i], led3[i]);
    }
    lcdR.setCursor(0, 0);
    lcdR.print("Please wait...  ");
    lcdR.setCursor(0, 1);
    lcdR.print("                ");
    for (int i = 0; i < 3; i++) {
      analogWrite(led1PinsR[i], 0);
      analogWrite(led2PinsR[i], 0);
      analogWrite(led3PinsR[i], 0);
    }
  } else {
    lcdR.setCursor(0, 0);
    lcdR.print(line1);
    lcdR.setCursor(0, 1);
    lcdR.print(line2);
    for (int i = 0; i < 3; i++) {
      analogWrite(led1PinsR[i], led1[i]);
      analogWrite(led2PinsR[i], led2[i]);
      analogWrite(led3PinsR[i], led3[i]);
    }
    lcdL.setCursor(0, 0);
    lcdL.print("Please wait...  ");
    lcdL.setCursor(0, 1);
    lcdL.print("                ");
    for (int i = 0; i < 3; i++) {
      analogWrite(led1PinsL[i], 0);
      analogWrite(led2PinsL[i], 0);
      analogWrite(led3PinsL[i], 0);
    }
  }
}

/*getSeatString(): get string describing current seat
 * 
 */
String getSeatString(){
  String seatstring = ""; 
  seatstring += String(1+currentRow);
  switch (drinksServed){
    case 0:
    seatstring += "A";
    break;
      case 1:
      
    seatstring += "B";
    break;
    case 2:
    
    seatstring += "C";
    break;
      case 3:
      
    seatstring += "D";
    break;
      case 4:
      
    seatstring += "E";
    break;
      case 5:
      
    seatstring += "F";
    break;
    default: 
    break; 
  }
  return seatstring; 
}

/******* STATUS MONITOR HELPERS ******/




/******* CAN NETWORK MESSAGE HELPERS *******/


/*drinkOrderUpdate(): update the drink order on the CAN network
*/
void drinkOrderUpdate() {
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER, drinkOrder);
  sendRunning = false;
}

/*readyToMoveUpdate(): update ready to move on the CAN network
*/
void readyToMoveUpdate() {
  char rtmMsg = 0; //this is FINE
  if (readyToMove == 1) {
    rtmMsg = 1;
  }

  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE, rtmMsg);
  sendRunning = false;

}

/*runningUpdate(): update running state on the CAN network
*/
void runningUpdate() {
  char runMsg = 0; //this is FINE
  if (nowRunning == 1) {
    runMsg = 1;
  }

  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_RUNNING, runMsg);
  sendRunning = false;

}

/*uiHealthUpdate(): update the ui health on the CAN network
*/
void uiHealthUpdate() {
  char healthMsg = 0; //this is FINE
  if (health == NOTFINE) {
    healthMsg = 1;
  }

  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_UI_HEALTH, healthMsg);
  sendRunning = false;

}

/*receivedMsgWrapper(): wrapper for receiving messages over
   the CAN network.
*/
void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.

  if (!sendRunning) {
    scl.receivedMsg();
  }

}



