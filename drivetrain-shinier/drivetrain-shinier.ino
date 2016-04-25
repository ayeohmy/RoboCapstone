//drivetrain-shinier.ino: so shiny drivetrain!!!
/*current status: only STATIONARY and MOVE are real states.
  PREPMOVE and SIDECLOSE have been bundled into move and it's
  easier this way
  STUCK needs to be a thing, it just isn't yet
*/

//problem state: CAN errors: 1111111
//???????????????CAN errors: 1111111

/*TODO:
   Timers
   motor control
   ultrasound listening
   encoder listening?
   stateflow
   write timer callbacks


*/

//things to include
#include <SquirtCanLib.h>
#include <SimpleTimer.h>

enum States {
  STATIONARY,
  PREPTOMOVE,
  MOVE,
  SIDECLOSE,
  STUCK
};

enum Health {
  FINE,
  HEALTH_STUCK
};


//pins to set
//int ultrasoundPins[5][2] = {{23, 24}, {25, 26}, {27, 28}, {29, 30}, {31, 32}};
int ultrasoundPins[5][2] = {{31, 32}, {23, 24}, {25, 26}, {27, 28}, {29, 30}, };

//{{trig1, echo1}, {trig2, echo2}, etc.}
//order: fc, fl, fr, bl, br
int encoderPins[4][2] = {{15, 16}, {17, 18}, {19, 20}, {33, 22}};
//{{A1,B1},{A2,B2}, etc.}
//order: f, l, r, b
int motorPins[4][3] = {{2, 7, 3}, {4, 8, 9}, {5, 12, 10}, {6, 13, 11}};
//{{INA1, INB1, PWM1}, {INA2, INB2, PWN2}, etc.}
//order: f, l, r, b
int slavePin = 53;
int interruptPin = 21;


//values we'll need to track
bool moving; //moving or not????
char atRow = 0; //if the row number exceeds 255 then we're in trouble/a huge plane
States state = STATIONARY;
Health health = FINE;
char msg;
char prevRtm = 0;
bool sendRunning = false;

//other constants to set
int motorSpeed = 100; // range: 0 to 255
int motorTime = 4000; //ms to drive

//objects to construct
SimpleTimer timer;
SquirtCanLib scl;


void setup() {
  // set up motors
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i][0], OUTPUT);
    pinMode(motorPins[i][1], OUTPUT);
    pinMode(motorPins[i][2], OUTPUT);
  }
  //set up ultrasounds
  for (int i = 0; i < 5; i++) {
    pinMode(ultrasoundPins[i][0], OUTPUT);
    pinMode(ultrasoundPins[i][1], INPUT);
  }

  //set up encoders?

  //set up CAN
  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, INPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, LOW);



  //set up timers, one for each message
 // timer.setInterval(25, movingUpdate);
 // timer.setInterval(50, atRowUpdate);
 // timer.setInterval(100, driveHealthUpdate);


  Serial.begin(9600);
  Serial.println("drivetrain setup done.");

}

void loop() {

  /*** things to always do ***/

  Serial.print("at row ");
  Serial.println((int)atRow);
  /*** things to sometimes do, depending on state ***/
  timer.run();

  switch (state) {
    case STATIONARY:
      {
        moving = false;
        //serial input goes here!!!
        char cmd = Serial.read();
        if (cmd == 'd') {
          state = MOVE;
          Serial.println("move yeaaaaa");
        }

        //wait for message to change to MOVE state
        char rtm = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_READY_TO_MOVE);
        Serial.print("ready to move? ");
        Serial.println((int) rtm);
        if (rtm && (prevRtm == 0)) {
          //if we just got an order, time to start preparing a drink!
          state = MOVE;
        }
        prevRtm = rtm;
        break;
      }
    case PREPTOMOVE:
      {
        stopMoving();
        //look at front ultrasound!
        int frontRange = getRange(ultrasoundPins[0]);
        break;
      }
    case MOVE:
      {
        moving = true;
        //try to move forward for the specified time
        //if necessary, pause until the thing in front of you moves
        Serial.println("moving!!! wheee");
        long lastTime = millis();
        long currTime = lastTime;
        long elapsedTime = 0;
        double frontRange = getRange(ultrasoundPins[0]);
        //Serial.print("frontrange: ");
        //Serial.println(frontRange);
        while (elapsedTime < motorTime) {
          timer.run(); //just in case
          frontRange = getRange(ultrasoundPins[0]);
          Serial.print("frontrange: ");
          Serial.println(frontRange);
          if (frontRange < 10) {
            stopMoving();
            Serial.println("stopped");
            currTime = millis();
            lastTime = currTime;
          } else {
            double flRange = 50;//getRange(ultrasoundPins[1]);
            double frRange = 50;//getRange(ultrasoundPins[2]);
            double blRange = 50; //getRange(ultrasoundPins[3]);
            double brRange = 50; //getRange(ultrasoundPins[4]);
            if ((flRange < 7 || frRange < 7)  ||
                (blRange < 7 || brRange < 7)) {
              turnAccordingly(flRange, frRange, blRange, brRange);
            } else {
              //everything's fine
              Serial.println("going forward...");
              forward(motorSpeed);
              currTime = millis();
              elapsedTime += currTime - lastTime;
              lastTime = currTime;
              Serial.print("elapsed time:");
              Serial.println(elapsedTime);
            }

          }

          delay(10); //to slow things down a bit
        }
        stopMoving();
        moving = false;
        atRow++;
        state = STATIONARY;

        break;
      }
    case SIDECLOSE:
      {

        break;
      }
    case STUCK:
      {
        stopMoving();
        break;
      }
    default:
      break;
  }//end state switch-statement


}




/*

 * * * * * HELPER FUNCTIONS

*/


/******** DRIVING HELPERS *********/



/*forward(SPD): drive forward with the specified speed (SPD).
   giving a negative value makes it go backwards.
   SPD is in the interval [-255 255].
*/
void forward(int spd) {
  setMotor(motorPins[1], -spd);
  setMotor(motorPins[2], spd);

}

/*back(SPD): drive backwards with the specified speed (SPD).
   just a wrapper around going forwards with negative input.
     SPD is in the interval [-255 255].
*/
void back(int spd) {
  forward(-spd);
}

/*strafe(SPD): strafe with the specified speed (SPD).
   positive speed strafes right, negative speed strafes left.
    SPD is in the interval [-255 255].
*/
void strafe(int spd) {
  //strafe right or left. positive == right, negative == left
  setMotor(motorPins[0], -spd);
  setMotor(motorPins[3], spd);
}

/*turn(SPD): turns with the specified speed (SPD).
   positive speed turns right, negative speed turns left.
    SPD is in the interval [-255 255].
*/
void turn (int spd) {
  //currently rotating with the front and back wheels only.
  setMotor(motorPins[0], -spd);
  setMotor(motorPins[3], -spd);
}

/*stopMoving(): stop all motors from moving.
*/
void stopMoving() {
  setMotor(motorPins[0], 0);
  setMotor(motorPins[1], 0);
  setMotor(motorPins[2], 0);
  setMotor(motorPins[3], 0);

}

/*setMotor(PINS,SPD): set the linear actuator motor to go the
   specified speed.
   input format: speed (+- int), pins ({inA,inB,pwm})
    SPD is in the interval [-255 255].
*/
void setMotor(int pins[], int spd) {

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

/*getRange(PINS): gets the range measurement from the
  ultrasound sensor denoted by PINS. PINS is a two-element
  array {t,e}, where t is the pin no. for the TRIG pin and
  e is the pin no. of the ECHO pin for this sensor.
*/
double getRange(int pins[]) {
  int trigPin = pins[0];
  int echoPin = pins[1];

  //get a range measurement, in cm, from an ultrasound sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  //default timeout is 1 second. i moved it up to 0.5?
  // Serial.println(duration);
  double dist = (duration / 2.0) / 29.14;
  return dist;
  // Serial.println(dist);
  //duration (in microseconds) is divided by two
  //because it gives the time to get there and back.
  //speed of sound = 29.14 microseconds per centimeter
}

void turnAccordingly(double flRange, double frRange,
                     double blRange, double brRange) {
  if (flRange < 7) {
    if (blRange < 7) {
      //strafe right if you're close on the whole left
      strafe(motorSpeed);
    } else {
      turn(motorSpeed);
    }
  }
  if (frRange < 7) {
    if (brRange < 7) {
      //strafe left if you're close on the whole right
      strafe(-motorSpeed);
    } else {
      turn(-motorSpeed);
    }
  }



}
/******* CAN NETWORK MESSAGE HELPERS *******/


/*movingUpdate(): update the moving status on the CAN network
*/
void movingUpdate() {

  char movingMsg = (char) moving;
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_MOVING, movingMsg);
  sendRunning = false;
}

/*atRowUpdate(): update the row status on the CAN network
*/
void atRowUpdate() {
  //Serial.println("updated atRow");
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW, atRow);
  sendRunning = false;

  Serial.println("updated atRow");
}


/*driveHealthUpdate(): update the drive health on the CAN network
*/
void driveHealthUpdate() {
  char healthMsg = 0; //this is FINE
  if (health == HEALTH_STUCK) {
    healthMsg = SquirtCanLib::ERROR_STUCK;
  }
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRIVE_HEALTH, healthMsg);
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






