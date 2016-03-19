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


#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //these are the lcd pins.
//(RS,enable, D4,D5,D6,D7)
//(4, 6, 11,12,13,14)
int buttonPins[] = {7, 8, 6};
int buttonStates[] = {0, 0, 0};

//all pins are the same for each LED color because
//we want to use an Uno, dangit
int led1Pins[] = {13, 14, 15}; //R,G,B
int led2Pins[] = {13, 14, 15}; //R,G,B
int led3Pins[] = {13, 14, 15}; //R,G,B

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
    //  pinMode(led1Pins[i], OUTPUT);
    //  pinMode(led2Pins[i], OUTPUT);
    //  pinMode(led3Pins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
  }

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
    panelDisplay("beep boop", "button 1!", RED, RED, RED);
    Serial.println("button 1");
  }
  if (!buttonStates[1]) {
    panelDisplay("bleep blop?", "button 2!", RED, RED, RED);
    Serial.println("button 2");
  }
/*  if (!buttonStates[2]) {
    //    panelDisplay("boop bloop","button 3!",RED,RED,RED);
  } 
  */
  if((buttonStates[0] && buttonStates[1])) {
    panelDisplay("beep boop", "*-*-*", GREEN, GREEN, GREEN);
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
