//drivetrain.ino: code for drivetrain.
//contact: Gillian Rosen, gtr@andrew.cmu.edu

//TODO: write everything.


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



#include <SquirtCanLib.h>

enum States {
  STATIONARY,
  PREPTOMOVE,
  MOVE,
  SIDECLOSE,
  STUCK
};

int slavePin = 5;
int interruptPin = 8;
int ultrasoundPins[5][2] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}};
//{{trig1, echo1}, {trig2, echo2}, etc.}
int encoderPins[4][2] = {{11, 12}, {13, 14}, {15, 16}, {17, 18}};
//{{A1,B1},{A2,B2}, etc.}
int motorPins[4][3] = {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}, {28, 29, 30}};
//{{INA1, INB1, PWM1}, {INA2, INB2, PWN2}, etc.}
States state = STATIONARY;
char origRow = 0;
char msg;
double botPose[] = {0, 0, 0}; //xpos,ypos,ori
double aisleWidth = 20; //hardcode this here or estimate it
SquirtCanLib scl;

void setup() {
  // put your setup code here, to run once:

  //navigation sensor setup
  for (int i = 0; i < 5; i++) {
    pinMode(ultrasoundPins[i][0], OUTPUT);
    pinMode(ultrasoundPins[i][1], INPUT);
  }
  //motor stuff setup
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i][0], OUTPUT);
    pinMode(motorPins[i][1], OUTPUT);
    pinMode(motorPins[i][2], OUTPUT);

    pinMode(encoderPins[i][0], INPUT);
    pinMode(encoderPins[i][1], INPUT);
  }

  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, OUTPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, RISING);
  msg = 0;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_MOVING, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH, msg);
}

void loop() {
  // put your main code here, to run repeatedly:

  //check pose to see if you're gonna turn or go straight 
        //estimate aisle pose
        double pose[3];
        estimatePose(pose);
        
        //if it's on-center enough, just go straight
        //otherwise, turn 
        if(pose[2] > 0.174533 
           || (abs(pose[0] - (aisleWidth/2)) > 2)){
            //0.174.. rad = 10 degrees
        
        }

  switch (state) {
    case STATIONARY:
      { 
        //maybe use this time to extimate yr aisle width? 
        break;
      }
    case PREPTOMOVE:
      {
        double frontDist = getRange(ultrasoundPins[0]);
        if (frontDist > 20) {
          //if there's nothing within 20 cm, then things are
          //probably fine?
          state = MOVE;
        } 
        break;
      }
    case MOVE:
      {
    
        //move forward 
        //setMotor(motorPins[1],10); //do for side wheels
        //setMotor(motorPins[3],10);
     
        break;
      }
    case SIDECLOSE:
      {

        //figure out if we need to turn or strafe or what 
        if (pose[2] > 0.174533){ //0.174.. rad = 10 degrees
        //turn a wee bit?
        //setMotor(motorPins[0],10); //do for all motors
        }
        if (abs(pose[0] - (aisleWidth/2)) > 2){
        //strafe a wee bit?

        }
        
        //exit this state? 
        break;
      }
    case STUCK:
      {
        msg = SquirtCanLib::ERROR_STUCK;
        scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH, msg);
        break;
      }
    default:
      break;
  }//end state switch-statement

}

double getRange(int pins[]) {
  int trigPin = pins[0];
  int echoPin = pins[1];
  //get a range measurement, in cm, from an ultrasound sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 0.5);
  //default timeout is 1 second. i moved it up to 0.5?
  double dist = (duration / 2.0) / 29.14;
  //duration (in microseconds) is divided by two
  //because it gives the time to get there and back.
  //speed of sound = 29.14 microseconds per centimeter
}

void setMotor(int pins[], int spd) {
  //input format: speed (+- int), pins ({inA,inB,pwm})
  if (spd >= 0) {
    //go forward. currently assuming this is clockwise.
    digitalWrite(pins[0], HIGH);
    digitalWrite(pins[1], LOW);
    analogWrite(pins[2], (byte)spd);
  } else {
    //go backward
    digitalWrite(pins[0], LOW);
    digitalWrite(pins[1], HIGH);
    analogWrite(pins[2], (byte)(-spd));
  }
}

void estimatePose(double pose[]) {
  //polls the four pose sensors and estimates robot pose.
  //puts result back in the array you passed in.
  //{xpos, ypos, ori}.

  //poll the sensorssssss

  //math to turn this into a pose

  //stuff it
  pose[0] = 1;
  pose[1] = 2;
  pose[2] = 3;

}

void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  scl.receivedMsg();
}


