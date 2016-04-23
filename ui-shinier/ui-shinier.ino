//ui-shiner.ino: ayyy shiny ui!

/*TODO:
 * deal with the "running" state. right now it's always running
 * literally all the status monitoring 
 */

//things to include
#include <SquirtCanLib.h>
#include <SimpleTimer.h>
#include <LiquidCrystal.h>


enum States{
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
int buttonPins[] = {23, 24, 22};
int led1Pins[] = {28, 29, 30}; //R,G,B
int led2Pins[] = {31, 32, 33}; //R,G,B
int led3Pins[] = {34, 35, 36}; //R,G,B
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


//other constants to set
const int GREEN[] = {0, 255, 0};
const int RED[] = {255, 0, 0};
const int BLUE[] = {255, 255, 255};
const int CYAN[] = {0, 255, 255};
const int MAGENTA[] = {255, 0, 255};
const int YELLOW[] = {255, 255, 0};
const int WHITE[] = {255, 255, 255};
const int BLACK[] = {0, 0, 0};

const String drinks[] = {"SQUIRT ", " WATER ", "  CIDER"};
//each of these must be 7 characters; pad as you want them


//objects to construct
LiquidCrystal lcd(14, 15, 10, 11, 12, 13); //these are the lcd pins.
//(RS,enable, D4,D5,D6,D7)
SquirtCanLib scl;
SimpleTimer timer;


void setup() {
  //set up LCD stuff
 lcd.begin(16, 2); // it's a 16x2 lcd
 
  //set up LEDs and buttons
    for (int i = 0; i < 3; i++) {
      pinMode(led1Pins[i], OUTPUT);
      pinMode(led2Pins[i], OUTPUT);
      pinMode(led3Pins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
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

  /*if(drinksServed == 3){
    drinksServed = 0; 
    state = DRIVING; 
    readyToMove = true; 
  }
  */
    timer.run();
  
  /*** things to sometimes do, depending on state ***/

  switch(state){
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
          //NOTE: actually
          String line1 = "    "; //padding spaces
          String line2 = "";    
            line1 += drinks[1];
            line2 += drinks[0];
          line2 += "|";
            line2 += drinks[2];

        //CHANGE THIS to actually actively update
          panelDisplay(line1, line2, CYAN, MAGENTA, YELLOW);


            for (int i = 0; i < 3; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);
  }
  if (!buttonStates[0]) {
      panelDisplay(line1, line2, CYAN, BLACK,BLACK);
      drinkOrder =(char) 1; 
      drinkTimestamp = millis(); 
      state = PREPARING; 
    
      
  }
  if (!buttonStates[1]) {
      panelDisplay(line1, line2, BLACK,MAGENTA,BLACK);
      drinkOrder =(char) 2; 
       drinkTimestamp = millis();
       state = PREPARING; 
     
      
  }
  if (!buttonStates[2]) {
               panelDisplay(line1, line2, BLACK,BLACK,YELLOW);
      drinkOrder =(char) 3;
       drinkTimestamp = millis(); 
       state = PREPARING; 
    
  } 
  
      break; 
    }
    case PREPARING: {
         panelDisplay("preparing drink ", "please wait...", RED,RED,RED);
      msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS);
      Serial.print("prep status:"); 
      Serial.println((int) msg);
      if(!msg && ((millis()-drinkTimestamp) > 5000)){
        //then we're done
        drinksServed++; 
         drinkOrder =(char) 0;
        state = REQUESTING; 
      }
      break; 
    }
    
    case DRIVING: {
       panelDisplay("driving,        ", "please wait...", BLUE, BLUE, BLUE);  
         msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW);
       Serial.print("at row:"); 
      Serial.println((int) msg);
 Serial.print("current row:"); 
      Serial.println(currentRow);  
      if(msg != currentRow){
        //then we're done
        currentRow = msg; 
        state = REQUESTING; 
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


void panelDisplay(String line1, String line2, const int led1[], const int led2[], const int led3[]) {
  //displays the given two lines and three LED colors on the UI panel
 // lcd.clear();
  while (line1.length() < 16){
    line1 += " "; 
  }
  while (line2.length() < 16){
    line2 += " "; 
  }    
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  for (int i = 0; i < 3; i++) {
    analogWrite(led1Pins[i], led1[i]);
    analogWrite(led2Pins[i], led2[i]);
    analogWrite(led3Pins[i], led3[i]);
  }
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
  if(readyToMove == 1) {
      rtmMsg= 1;
  }

  sendRunning = true; 
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE, rtmMsg);
  sendRunning = false; 

}

/*runningUpdate(): update running state on the CAN network
*/
void runningUpdate() {
    char runMsg = 0; //this is FINE
  if(nowRunning == 1) {
      runMsg= 1;
  }

  sendRunning = true; 
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_RUNNING, runMsg);
  sendRunning = false; 

}

/*uiHealthUpdate(): update the ui health on the CAN network
*/
void uiHealthUpdate() {
  char healthMsg = 0; //this is FINE
  if(health == NOTFINE) {
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

if (!sendRunning){ 
  scl.receivedMsg();
}

}



