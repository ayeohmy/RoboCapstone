//testStack: test the cup stack & grabber
//TODO: test iiiiiiiit

//NOTE: up and down for the stepper are reversed; see stepperGo()

#include "DRV8825.h"


//HOW TO MOTORS: (from pololu motor driver site)
// INA pin is clockwise input, INB pin is counterclockwise
// so if you want to go clockwise:
//    digitalWrite(inA, HIGH); digitalWrite(inB,LOW);
//and if you want to go counterclockwise:
//    digitalWrite(inA, LOW); digitalWrite(inB, HIGH);
//to do speed control, analog write a byte value to the PWM pin.
//this works because arduino analog write on digital pin
//is actually pwm.
//NOTE: pwm frequency is ~490Hz on most pins, but ~980Hz on
//pins 5 and 6 because of internal Arduino timing shenanigans.
// this pwm writing works on pins 2-13 and 44-46 on the Mega.
//to do speed:
//    analogWrite(pwmPin, speed);  //speed is a byte 0-255

#define STEPPER 0
#define LINEAR 1

bool runStack = true;
bool runGrab = true;
bool danger = true;

int grabberMotorPins[] = {8, 10, 9}; //{ina, inb, pwm}. DO NOT put pwm on 5 or 6!
int grabberLimitSwitchPins[] = {6, 7}; //pins for grabber limit switches {in, out}
int stackLimitSwitchPins[] = {4, 5}; //pins for stack limit switches {bottom, top}
int stackMotorPins[] = {2, 3}; //{direction, step} pins

int degsUp = 180;
int degsDown = 180;
int rpms = 400;

int motorTime = 500; //ms
int motorSpeed = 220;
DRV8825 stepper(200, stackMotorPins[0], stackMotorPins[1]); //steps/rev, dir, step


void setup() {
  // put your setup code here, to run once:
  if (runStack) {

    pinMode(stackLimitSwitchPins[0], INPUT);
    pinMode(stackLimitSwitchPins[1], INPUT);


    //set up stepper motor
    stepper.setRPM(rpms); //1 RPM i guess?
    stepper.setMicrostep(1); // nope on microstepping
  }

  if (runGrab) {
    pinMode(grabberMotorPins[0], OUTPUT);
    pinMode(grabberMotorPins[1], OUTPUT);
    pinMode(grabberMotorPins[2], OUTPUT);

    pinMode(grabberLimitSwitchPins[0], INPUT);
    pinMode(grabberLimitSwitchPins[1], INPUT);


  }
  Serial.begin(9600);
  Serial.print("runStack:");
  Serial.println(runStack);
  Serial.print("runGrab:");
  Serial.println(runGrab);
  Serial.print("live dangerously?");
  Serial.println(danger);
  Serial.println("w to raise stack, s to lower stack, a to extend arm, d to retract arm");

}

void loop() {
  // put your main code here, to run repeatedly:


  char cmd = Serial.read();
  //Serial.println(cmd);
  switch (cmd) {
    case 'w': {
        if (danger) {
          stepperGoDangerously(degsUp);
          Serial.println("stack up (dangerously)");
        } else {
          stepperGo(degsUp);
          Serial.println("stack up");
        }

        break;
      }
    case 'a': {
        if (danger) {
          linearGoDangerously(-1);
        } else {

        }
        Serial.println("grabber in");
        break;
      }
    case 's': {
        if (danger) {
          stepperGoDangerously(-degsDown);

          Serial.println("stack down(dangerously)");
        } else {
          stepperGo(-degsDown);

          Serial.println("stack down");
        }
        break;
      }
    case 'd': {
        if (danger) {
          linearGoDangerously(1);
        } else {

        }
        Serial.println("grabber out");
        break;
      }

    case 'i': {
        //stepper run up continuously
        runContinuously(1, STEPPER);
        break;
      }
    case 'k': {
        //stepper run down continuously
        runContinuously(-1, STEPPER);
        break;
      }
    case 'j': {
        //grabber run in continuously
        runContinuously(-1, LINEAR);
        break;
      }
    case 'l': {
        //grabber run out continuously
        runContinuously(1, LINEAR);
        break;
      }
    case 'm': {
        //check limit pins
        Serial.println("limit pins:");
        bool lim = digitalRead(grabberLimitSwitchPins[0]);
        Serial.print("grabber in:");
        Serial.println(lim);
        lim = digitalRead(grabberLimitSwitchPins[1]);
        Serial.print("grabber out:");
        Serial.println(lim);
        lim = digitalRead(stackLimitSwitchPins[0]);
        Serial.print("stack bottom:");
        Serial.println(lim);
        lim = digitalRead(stackLimitSwitchPins[1]);
        Serial.print("stack top:");
        Serial.println(lim);
        break;
      }


    default:
      linearGoDangerously(0);
      break;
  }
  delay(100); //10 Hz-ish is fine? how does delay() even work

}

void stepperGoDangerously(int degs) {
  stepper.rotate(-degs);

}

void linearGoDangerously(int howso) {

  switch (howso) {
    case -1:
      //go in
      setMotor(grabberMotorPins, -motorSpeed);
      delay(motorTime);
      setMotor(grabberMotorPins, 0);
      break;
    case 1:
      setMotor(grabberMotorPins, motorSpeed);
      delay(motorTime);
      setMotor(grabberMotorPins, 0);
      break;
    default:
      setMotor(grabberMotorPins, 0);
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

int runContinuously(int dir, bool motor) {
  //runs the motor continuously until we hit the limit switch.
  //1 for up/out, -1 for down/in
  
  //note: stepper direction is reversed!!!! 


  
  int pins[2];
  if (motor == STEPPER) {
    pins[0] = stackLimitSwitchPins[0];
    pins[1] = stackLimitSwitchPins[1];
  } else {
    pins[0] = grabberLimitSwitchPins[0];
    pins[1] = grabberLimitSwitchPins[1];
  }

  int limitPin = pins[0]; //if dir == -1 then down/in
  if (dir == 1) limitPin = pins[1]; //if dir == 1 then up/out

  bool limSwitch = digitalRead(limitPin);
  int degsSoFar = 0;
  while (limSwitch) {
    if (motor == STEPPER) {
      stepper.rotate((-1*dir) * 60);
      degsSoFar += 60;
    } else { //motor == LINEAR, i'm assuming
      setMotor(grabberMotorPins, dir * 220);
      delay(200);
    }
    limSwitch = digitalRead(limitPin);
  }
  if (motor == STEPPER){
   // delay(20);
  //  stepper.rotate((dir) * 90); //back up a lil bit
  } else { //motor == LINEAR
    setMotor(grabberMotorPins, 0);
  }
  setMotor(grabberMotorPins, 0);
  
  return dir * degsSoFar;
}



int getCup() {
  //gets a cup from the stack.
  //returns 0 if everything's OK.

  //stack actuate up
  int degsUp = runContinuously(1, STEPPER);
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


void setMotor(int pins[], int spd) {
  //input format: speed (+- int), pins ({inA,inB,pwm})
  if (spd >= 0) {
    //go forward. currently assuming this is clockwise.
    digitalWrite(pins[0], LOW);
    digitalWrite(pins[1], HIGH);
    analogWrite(pins[2], (byte)spd);
  } else {
    //go backward
    digitalWrite(pins[0], HIGH);
    digitalWrite(pins[1], LOW);
    analogWrite(pins[2], (byte)(-spd));
  }
}
