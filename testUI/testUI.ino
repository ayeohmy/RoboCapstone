//testUI: test that UI electronics are happy.


//CURRENT ACTUAL PINS:
/*
   0: nothing
   1: nothing
   2: INT on CAN chip (interruptPin?)
   3: nothing
   4: nothing
   5: valve 2
   6: valve 1
   7: valve 3
   8: pressure sensor (?)
   9:  compressor + (?)
   10: CS on CAN chip (slavePin?)
   11: SI on CAN chip (data input?)
   12: SO on CAN chip  (data output?)
   13: SCK on CAN chip (clock?)

*/


#include <SquirtCanLib.h>

SquirtCanLib scl;

int slavePin = 53;
int interruptPin = 21;
char msg;

#include <LiquidCrystal.h>
LiquidCrystal lcd(20, 7, 16,17,18,19); //these are the lcd pins.
//(RS,enable, D4,D5,D6,D7)

int buttonPins[] = {25,26,27};
int buttonStates[] = {0, 0, 0};

//all pins are the same for each LED color because
//we want to use an Uno, dangit
int led1Pins[] = {28, 29, 30}; //R,G,B
int led2Pins[] = {31, 32, 33}; //R,G,B
int led3Pins[] = {34, 35, 36}; //R,G,B

//for setting LED colors
const int GREEN[] = {0, 255, 0};
const int RED[] = {255, 0, 0};
const int WHITE[] = {255, 255, 255};
const int BLACK[] = {0, 0, 0};

const String drinks[] = {"SQUIRT ", " WATER ", "  CIDER"};
//each of these must be 7 characters; pad as you want them

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2); // it's a 16x2 lcd
  for (int i = 0; i < 3; i++) {
      pinMode(led1Pins[i], OUTPUT);
      pinMode(led2Pins[i], OUTPUT);
      pinMode(led3Pins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
  }


  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);


  //Serial.setTimeout(500); //to make things a bit faster
  Serial.begin(9600);
  Serial.println("beep!");
  //  lcd.print("beep boop");
  panelDisplay("beep boop", "*-*-*", GREEN, GREEN, GREEN);
}

void loop() {

  // put your main code here, to run repeatedly:
  // panelDisplay("beep boop","*-*-*",GREEN,GREEN,GREEN);
  /*
    Serial.println("input things:");
    String msg = Serial.readStringUntil('\n');

    if (msg.length() > 0){
     panelDisplay(msg,"...",RED,RED,RED);

    }*/
  for (int i = 0; i < 3; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);
  }
  if (!buttonStates[0]) {
    panelDisplay("beep booooop", "button 1- red!", RED, RED, RED);
    Serial.println("button 1");
      scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER,(char) 1); 
  }
  if (!buttonStates[1]) {
    panelDisplay("bleep blop?", "button 2- green!", GREEN, GREEN, GREEN);
    Serial.println("button 2");
      scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER,(char) 2); 

  }
  if (!buttonStates[2]) {
        panelDisplay("boop bloop","button 3- white!",WHITE, WHITE, WHITE);
         Serial.println("button 3");
           scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER,(char) 3); 

  } 
  
  if(buttonStates[2] && (buttonStates[0] && buttonStates[1])) {
    panelDisplay("beep boop", "*-*-*", BLACK, BLACK, BLACK);
    scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER,(char) 0); 
  
  }

  // lcd.setCursor(0, 1);
  // lcd.print("beep boop");
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);

  // delay(100);
}


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
    analogWrite(led2Pins[i], led1[i]);
    analogWrite(led3Pins[i], led1[i]);
  }
}



void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}
