//testStack: test the cup stack & grabber
//to think of: put rubberized lining on the gripper inside

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


int grabberMotorPins[] = {8, 10, 9}; //{ina, inb, pwm}. DO NOT put pwm on 5 or 6!
int grabberLimitSwitchPins[] = {6, 7}; //pins for grabber limit switches {in, out}
int stackLimitSwitchPins[] = {4, 5}; //pins for stack limit switches {bottom, top}
int stackMotorPins[] = {2, 3}; //{direction, step} pins

int degsUp = 180;
int degsDown = 180;
int rpms = 400;

int motorTime = 500; //ms
int motorSpeed = 250;
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
  Serial.println("w to raise stack, s to lower stack, a to extend arm, d to retract arm");

}

void loop() {
  // put your main code here, to run repeatedly:


  char cmd = Serial.read();
  //Serial.println(cmd);
  switch (cmd) {
    case 'w': {
          setStepper(degsUp);
          Serial.println("stack up ");
        break;
      }
    case 'a': {
        
          linearGoDangerously(-1);
        Serial.println("grabber in");
        break;
      }
    case 's': {
          setStepper(-degsDown);
          Serial.println("stack down");
        break;
      }
    case 'd': {
       
          linearGoDangerously(1);
        Serial.println("grabber out");
        break;
      }

    case 'i': {
        //stepper run up continuously
        int howfar = runContinuously(1, STEPPER);
        Serial.println(howfar); 
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
    case 'g': {
        //get a cup
        getCup();
        break;
      }

    default:
      linearGoDangerously(0);
      break;
  }
  delay(100); //10 Hz-ish is fine? how does delay() even work

}

void setStepper(int degs) {
//set the stepper to go the specified amount of degrees. 
//note that the stepper direction is reversed, so we need to
//flip the sign of the input. 
  stepper.rotate(-degs);

}

void linearGoDangerously(int howso) {
//run the linear actuator in the specified direction; 
//-1 to go in, 1 to go out. for any other input, stops the motor. 
//runs for the amount of time specified by the global motorTime,
//at the speed specified by global motorSpeed.  
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

int runContinuously(int dir, bool motor) {
  //runs the specified motor (motor) continuously
  //in the specified direction (dir) until we hit the 
  //corresponding limit switch.
  //1 for up/out, -1 for down/in
 
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
      setStepper(dir * 60);
      degsSoFar += 60;
    } else { //motor == LINEAR, i'm assuming
      setMotor(grabberMotorPins, dir * 220);
      delay(200);
    }
    limSwitch = digitalRead(limitPin);
  }
  if (motor == STEPPER){
   // delay(20);
  //  setStepper(-dir * 90); //back up a lil bit
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
  runContinuously(1,LINEAR); 

  //stack actuate down

  setStepper(-3660); //empirically measured cup height 

  //reach in
  runContinuously(-1,LINEAR); 

  return 0;
}


void setMotor(int pins[], int spd) {
  //input format: speed (+- int), pins ({inA,inB,pwm})
  if (spd >= 0) {
    //go forward. for the linear actuator, this is counterclockwise.
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
