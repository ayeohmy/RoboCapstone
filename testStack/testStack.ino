//testStack: test the cup stack & grabber
//TODO: test iiiiiiiit


#include "DRV8825.h"

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

bool runStack = true;
bool runGrab = false;
bool danger = true;

int grabberMotorPins[] = {2, 10}; //i THINK we need two pins, one pwr one gnd
int grabberLimitSwitchPins[] = {11, 12}; //pins for grabber limit switches {in, out}
int stackLimitSwitchPins[] = {7, 8}; //pins for stack limit switches {bottom, top}
int stackMotorPins[] = {1, 13}; //{direction, step} pins

int degsUp = 360;
int degsDown = 270;
DRV8825 stepper(200, 1, 13); //steps/rev, dir, step


void setup() {
  // put your setup code here, to run once:
  if (runStack) {

    pinMode(stackLimitSwitchPins[0], INPUT);
    pinMode(stackLimitSwitchPins[1], INPUT);


    //set up stepper motor
    stepper.setRPM(1); //1 RPM i guess?
    stepper.setMicrostep(1); // nope on microstepping
  }

  if (runGrab) {
    pinMode(grabberMotorPins[0], OUTPUT);
    pinMode(grabberMotorPins[1], OUTPUT);
    pinMode(grabberLimitSwitchPins[0], INPUT);
    pinMode(grabberLimitSwitchPins[1], INPUT);

    //tell the linear actuator to not go yet
    digitalWrite(grabberMotorPins[0], LOW);
    digitalWrite(grabberMotorPins[1], LOW);
  }
  Serial.println("w to raise stack, s to lower stack, a to extend arm, d to retract arm");

}

void loop() {
  // put your main code here, to run repeatedly:


  char cmd = Serial.read();
  //Serial.println(cmd);
  switch (cmd) {
    case 'w': {
        if (danger) {
          stepperGoDangerously(360);
        } else {
          stepperGo(360);
        }
        Serial.println("stack up");
        break;
      }
    case 'a': {
         if (danger) {
          linearGoDangerously(1);
        } else {
         
        }
        Serial.println("grabber out");
        break;
      }
    case 's': {
        if (danger) {
          stepperGoDangerously(-360);
        } else {
          stepperGo(-360);
        }
        Serial.println("stack down");
        break;
      }
    case 'd': {
         if (danger) {
          linearGoDangerously(-1);
        } else {
         
        }
        Serial.println("grabber in");
        break;
      }

    default:
     linearGoDangerously(0);
      break;
  }
  delay(100); //10 Hz-ish is fine? how does delay() even work

}

void stepperGoDangerously(int degs) {
  stepper.rotate(degs);

}

void linearGoDangerously(int howso) {
  switch (howso) {
    case -1:
      //go in
      digitalWrite(grabberMotorPins[0], LOW);
      digitalWrite(grabberMotorPins[1], HIGH);
      break;
    case 1:
      digitalWrite(grabberMotorPins[0], HIGH);
      digitalWrite(grabberMotorPins[1], LOW);
      break;
    default:
      digitalWrite(grabberMotorPins[0], LOW);
      digitalWrite(grabberMotorPins[1], LOW);
      break;
  }
}

void stepperGo(int degs) {
  if (degs > 0) {

    // want/need to check continuously while rotating?
    int degsSoFar = 0;
    while (degsSoFar < degs) {
      Serial.print("degsSoFar: ");
      Serial.println(degsSoFar);
      int limitCheck = digitalRead(stackLimitSwitchPins[1]);
      if (limitCheck) {
        if (degs - degsSoFar > 90) {
          //go 90
          stepper.rotate(90);
        } else {
          //go the remaining distance
          stepper.rotate(degs - degsSoFar);
        }
      }
      degsSoFar += 90;  //incrementing by 90 means... how far?
    }
  } else {
    //check bottom limit switch to go down
    int degsSoFar = 0;
    while (degsSoFar > degs) {
      Serial.print("degsSoFar: ");
      Serial.println(degsSoFar);
      int limitCheck = digitalRead(stackLimitSwitchPins[0]);
      if (limitCheck) {
        if (degs - degsSoFar < -90) {
          //go 90
          stepper.rotate(-90);
        } else {
          //go the remaining distance
          stepper.rotate(degs - degsSoFar);
        }
      }

      degsSoFar -= 90;  //incrementing by 90 means... how far?
    }
  }

}

int getCup() {
  //gets a cup from the stack.
  //returns 0 if everything's OK.

  //stack actuate up
  stepper.rotate(degsUp); //rotate 360 degrees. figure out empirically
                    //how much rotation we actually need per action.
                    
  //reach out
  int outSwitch = digitalRead(grabberLimitSwitchPins[1]);
  int inSwitch = digitalRead(grabberLimitSwitchPins[0]);
  while (!outSwitch) {
    digitalWrite(grabberMotorPins[0], HIGH);
    digitalWrite(grabberMotorPins[1], LOW);

    outSwitch = digitalRead(grabberLimitSwitchPins[0]);
  }
       //stop movin
  digitalWrite(grabberMotorPins[0], LOW);
  digitalWrite(grabberMotorPins[1], LOW);

  //stack actuate down

 stepper.rotate(degsDown); //rotate -300 degrees. figure out empirically
                    //how much rotation we actually need per action.
                    

  //reach in
  while (!inSwitch) {
    digitalWrite(grabberMotorPins[0], LOW);
    digitalWrite(grabberMotorPins[1], HIGH);

    inSwitch = digitalRead(grabberLimitSwitchPins[1]);
  }

  //stop movin
  digitalWrite(grabberMotorPins[0], LOW);
  digitalWrite(grabberMotorPins[1], LOW);


  return 0;
}

