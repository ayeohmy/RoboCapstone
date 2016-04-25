//drinkPrep-shinier.ino: so shiny drink prep!!!

/*TODO:
  fix it so you can order a drink when the compressor is on!!!!!


*/

/*NOTES:
   HIGH on the pressure sensor means that the pressure is LOW.
   so if it's HIGH/1, then you need to turn it on!
    up and down for the stepper are reversed; see setStepper()

*/
//things to include
#include <SquirtCanLib.h>
#include <SimpleTimer.h>
#include "DRV8825.h"
#include "../libraries/watchdog.cpp"

enum States {
  WAITING,
  PREPARING
};

enum Health {
  FINE,
  NOCUPS,
  NODRINKS
};


#define STEPPER 0
#define LINEAR 1


//pins to set
int grabberMotorPins[] = {8, 10, 9}; //{ina, inb, pwm}. DO NOT put pwm on 5 or 6!
int grabberLimitSwitchPins[] = {6, 7}; //pins for grabber limit switches {in, out}
int stackLimitSwitchPins[] = {4, 5}; //pins for stack limit switches {bottom, top}
int stackMotorPins[] = {2, 3}; //{direction, step} pins
int valvePins[] = {11, 12, 13};
int pressureSensorPin = 14;
int compressorPin = 15;
int ultrasoundPins[] = {16, 17}; //{trig, echo}
int slavePin = 53;
int interruptPin = 21;


//values we'll need to track
int stock[] = {10, 10, 10, 30}; //{d1, d2, d3, cups}
char msg;
States state = WAITING;
Health health = FINE;
char prevDrinkOrder = 0;
bool sendRunning = false;


//other constants to set
int degsUp = 180;
int degsDown = 180;
int rpms = 400;
int motorTime = 500; //ms
int motorSpeed = 250; //0-255
double maxDrinkDist = 11.0; //let's say when we're 11.0cm from the drink, we stop

int looptime;
int lastLooptime;

bool oops = true;

//objects to construct
SimpleTimer timer;
SquirtCanLib scl;
DRV8825 stepper(200, stackMotorPins[0], stackMotorPins[1]); //steps per rev, dir pin, step pin

void setup() {
  // set up stack stuff
  pinMode(stackLimitSwitchPins[0], INPUT);
  pinMode(stackLimitSwitchPins[1], INPUT);
  stepper.setRPM(rpms);
  stepper.setMicrostep(1); // nope on microstepping
  pinMode(grabberMotorPins[0], OUTPUT);
  pinMode(grabberMotorPins[1], OUTPUT);
  pinMode(grabberMotorPins[2], OUTPUT);
  pinMode(grabberLimitSwitchPins[0], INPUT);
  pinMode(grabberLimitSwitchPins[1], INPUT);

  //watchdog_init();

  //set up pneumatics
  pinMode(ultrasoundPins[0], OUTPUT);
  pinMode(ultrasoundPins[1], INPUT);
  pinMode(valvePins[0], OUTPUT);
  pinMode(valvePins[1], OUTPUT);
  pinMode(valvePins[2], OUTPUT);
  pinMode(pressureSensorPin, INPUT);
  pinMode(compressorPin, OUTPUT);


  //set up CAN
  scl.canSetup(slavePin); //pass in slave select pin
  pinMode(interruptPin, INPUT); //this pin is for the general interrupt line for the CAN chip
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, LOW);
  msg = 0;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH, msg);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_STOCK_STATUS, msg);
  pinMode(49, OUTPUT);

  //set up timers, one for each message
  timer.setInterval(50, stockStatusUpdate);
  timer.setInterval(50, prepStatusUpdate);
  timer.setInterval(100, prepHealthUpdate);


  Serial.begin(9600);
  Serial.println("drinkPrep setup done.");
}

void loop() {
  // looptime = millis();

  /*** things to always do ****/

  //wdt_reset();

  //keep the system pressurized, always
  //if the thing is low pressure, turn on the compressor
  //otherwise don't
  volatile int needPressure = digitalRead(pressureSensorPin);
  // Serial.println(needPressure);
  if (needPressure == 1) {
    //digitalWrite(compressorPin, HIGH);
    //Serial.println("compressor ON");
  } else {
    digitalWrite(compressorPin, LOW);
    //  Serial.println("compressor OFF");
  }

  //timer maintenance
  // Serial.println("updating timer");
  timer.run();


  //check if it's time to make a drink
  // Serial.println("trying to get drink order...");
  // digitalWrite(49,HIGH);
  char drinkOrder = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER);

  Serial.print("drinkorder: ");
  Serial.println((int) drinkOrder);
  if (drinkOrder != 0 && prevDrinkOrder == 0) {
    //if we just got an order, time to start preparing a drink!
    state = PREPARING;
  }

  prevDrinkOrder = drinkOrder;
  Serial.print("state:");
  Serial.println(state);
  msg = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_AT_ROW);
  Serial.print("at row:");
  Serial.println((int) msg);

  /*** things to sometimes do, depending on state ***/



  if (state == PREPARING) {
    /*
       PREPARING
    */
    Serial.println("prepping a drink!");
    //parse le drink order
    //FOR NOW: fake drink order to 2
    //drinkOrder = 2;
    int whichDrink = parseOrder(drinkOrder);

    //dispense le drink based on order
    getCup();
    Serial.println("cup retrieval done.");
    dispenseDrink(whichDrink);
    Serial.println("drink dispensing done.");
    //update stocks
    stock[3] = stock[3] - 1; //less one cup
    stock[whichDrink - 1] = stock[whichDrink - 1] - 1;

    //update health
    if (stock[3] <= 0) {
      health = NOCUPS;
    }
    if (stock[0] <= 0  && (stock[1] <= 0 && stock[2] <= 0)) {
      health = NODRINKS;
    }
    //update state; go back to waiting
    state = WAITING;
    Serial.println("drink done!");
  } else { //state must be WAITING
    /*
       WAITING
    */
    //i guess we don't have anything to do here idly, since we
    //have no stock sensing....

    //so we'll do keyboard commands!
    //digitalWrite(49,HIGH);
    char keyIn = Serial.read();

    // digitalWrite(49,LOW);
    //  Serial.println(keyIn);

    switch (keyIn) {
      case '0': {
          Serial.println("close all");
          closeValves();
          break;
        }
      case '1': {
          Serial.println("open 1");
          openValve(1);
          break;
        }
      case '2': {
          Serial.println("open 2");
          openValve(2);
          break;
        }
      case '3': {

          Serial.println("open 3");
          openValve(3);
          break;
        }
      case '4': {
          //dispense drink 1
          Serial.println("dispense drink 1");
          dispenseDrink(1);
          break;
        }
      case '5': {
          //dispense drink 2
          Serial.println("dispense drink 2");
          dispenseDrink(2);
          break;
        }
      case '6': {
          //dispense drink 3
          Serial.println("dispense drink 3");
          dispenseDrink(3);
          break;
        }
      case 'w': {
          Serial.println("stack up ");
          setStepper(degsUp);
          break;
        }
      case 'a': {
          Serial.println("grabber in");
          setMotor(grabberMotorPins, -motorSpeed);
          delay(motorTime);
          setMotor(grabberMotorPins, 0);
          break;
        }
      case 's': {
          Serial.println("stack down");
          setStepper(-degsDown);

          break;
        }
      case 'd': {

          Serial.println("grabber out");
          setMotor(grabberMotorPins, motorSpeed);
          delay(motorTime);
          setMotor(grabberMotorPins, 0);
          break;
        }

      case 'i': {
          Serial.println("stepping up");
          //stepper run up continuously
          int howfar = runContinuously(1, STEPPER);
          break;
        }
      case 'k': {
          Serial.println("stepping down");
          //stepper run down continuously
          runContinuously(-1, STEPPER);

          break;
        }
      case 'j': {
          //grabber run in continuously
          Serial.println("grabbing in");
          runContinuously(-1, LINEAR);

          break;
        }
      case 'l': {
          //grabber run out continuously
          Serial.println("grabbing out");
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
          Serial.println("getting a cup...!" );
          getCup();
          break;
        }
      case 'p': {
          Serial.println("switching state to PREPARING");
          state = PREPARING;
          break;
        }
      default:
        break;

    }
  }
  // Serial.println("loop done");
  /* lastLooptime = looptime;
    looptime = millis();
    Serial.print("loop time:");
    Serial.println(looptime - lastLooptime);
  */
  /*INT8U err = scl.checkErr();
    Serial.print("err: ");
    Serial.print(err,BIN);
    Serial.print("\n");
  */
}




/*

 * * * * * HELPER FUNCTIONS

*/




/******* CUP PLACEMENT HELPERS ********/



/*setStepper(DEGS): set the stepper to go the specified
   amount of degrees.
*/
void setStepper(int degs) {
  //note that the stepper direction is reversed, so we need to
  //flip the sign of the input.
  stepper.rotate(-degs);

}

/*setMotor(PINS,SPD): set the linear actuator motor to go the
   specified speed.
   input format: speed (+- int), pins ({inA,inB,pwm})
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

/*runContinuously(DIR, MOTOR): runs the specified motor MOTOR
   continuously in the specified direction DIR until we hit
   the corresponding limit switch.
   DIR format: 1 for up/out, -1 for down/in
*/
int runContinuously(int dir, bool motor) {

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
      setMotor(grabberMotorPins, dir * motorSpeed);
      delay(100);
      timer.run();
    }
    limSwitch = digitalRead(limitPin);

    /*  char cmd = Serial.read();
      if (cmd == ' ') {
        //STOP....
        Serial.println("STOPPIN");
        limSwitch = 0;
      }
    */
  }
  if (motor == STEPPER) {
    delay(300);
    setStepper(-dir * 80); //back up a lil bit
  }

  setMotor(grabberMotorPins, 0);

  return dir * degsSoFar;
}

/*getCup(): run the full process of getting a cup, including
   raising the cup stack, extending the arm, lowering the
   cup stack, and retracting the arm.
*/
int getCup() {
  //gets a cup from the stack.
  //returns 0 if everything's OK.

  //stack actuate up
  int degsUp = runContinuously(1, STEPPER);

  //reach out
  runContinuously(1, LINEAR);

  //stack actuate down

  setStepper(-3660); //empirically measured cup height

  //reach in
  runContinuously(-1, LINEAR);

  return 0;
}



/******* PNEUMATICS HELPERS ***********/



/*dispenseDrink(DRINKNO): dispenses the specified drink for
   DRINKNO == 1, 2, or 3. for any other value of drinkNo,
   does nothing.
*/
void dispenseDrink(int drinkNo) {
  //dispenses the specified drink (drinkNo == 1, 2, or 3).
  //for any other value of drinkNo, does nothing.
  if (drinkNo < 1 || drinkNo > 3) return;


  // Serial.print("dispensing ");
  // Serial.println(drinkNo);
  double drinkDist = getRange(ultrasoundPins);
  Serial.print("drink distance: ");
  Serial.println(drinkDist);
  long timecheck0 = millis();
  long timecheck = timecheck0;
  //while the drink's height is below the threshold height,
  //and time is not too long,
  //keep dispensing

  while ((drinkDist > maxDrinkDist)
         && (timecheck - timecheck0 < 5000)) {
    openValve(drinkNo);
    timer.run();
    delay(50); //so we don't loop too tightly
    timecheck = millis();
    drinkDist = getRange(ultrasoundPins);

    Serial.print("drink distance: ");
    Serial.println(drinkDist);
    /*
        char cmd = Serial.read();
        if (cmd == ' ') {
          //STOP....
          Serial.println("STOPPIN");
          drinkDist = maxDrinkDist;
        }
    */
  }

  //stop dispensing
  Serial.print("done dispensing");
  Serial.println(drinkNo);
  closeValves();
}

/*openValve(VALVENO): opens the specified valve for
   VALVENO == 1, 2, or 3. for any other value of valveNo,
   shuts all valves.
*/
void openValve(int valveNo) {
  switch (valveNo) {
    case 1: {
        digitalWrite(valvePins[0], HIGH);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        break;
      }
    case 2: {
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], HIGH);
        digitalWrite(valvePins[2], LOW);
        break;
      }
    case 3: {
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], HIGH);
        break;
      }
    default: {
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        break;
      }
  }


}

/*closeValves(): closes all valves.
*/
void closeValves() {
  //closes all valves.
  openValve(0);

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



/******* CAN NETWORK MESSAGE HELPERS *******/



/*stockStatusUpdate(): update the stock status on the CAN network
*/
void stockStatusUpdate() {
  //  Serial.println("updating stock status");

  //pack the message
  char stockMsg = 0x00;
  if (stock[0] > 0 ) {
    stockMsg = stockMsg | 0x80; //1000 0000
  } //else it's already at zeros
  if (stock[1] > 0 ) {
    stockMsg = stockMsg | 0x40; //0100 0000
  } //else it's already at zeros
  if (stock[2] > 0 ) {
    stockMsg = stockMsg | 0x20; //0010 0000
  } //else it's already at zeros
  if (stock[3] > 0 ) {
    stockMsg = stockMsg | 0x10; //0001 0000
  } //else it's already at zeros
  //Serial.println("abt to send the stock stat msg...!");
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_STOCK_STATUS, stockMsg);
  sendRunning = false;
  // Serial.println("updated stock status");
}


/*prepStatusUpdate(): update the prep status on the CAN network
*/
void prepStatusUpdate() {
  //  Serial.println("updating prep status");
  char statMsg = 0; //this is WAITING
  if (state == PREPARING) {
    statMsg = 1;
  }

  // Serial.println("abt to send the prep stat msg...!");
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS, statMsg);
  sendRunning = false;
  //Serial.println("updated prep status");
}


/*prepHealthUpdate(): update the prep health on the CAN network
*/
void prepHealthUpdate() {
  // Serial.println("updating prep health");
  char healthMsg = 0; //this is FINE
  switch (health) {
    case NOCUPS:
      healthMsg = SquirtCanLib::ERROR_NOCUPS;
      break;
    case NODRINKS:
      healthMsg = SquirtCanLib::ERROR_NODRINKS;
      break;
    default:
      break;
  }

  // Serial.println("abt to send the prep health msg...!");
  sendRunning = true;
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_HEALTH, healthMsg);
  sendRunning = false;
  //Serial.println("updated prep health");
}


/*parseOrder(DRINKORDER): parses the drink order, returning
   an int corresponding to the ordered drink.
*/
int parseOrder(char drinkOrder) {
  //IDEA: which drink you want is just an unsigned char number.
  // leading bit is then added on top; it denotes the side.

  //remove the 'side' bit to get to the good content
  drinkOrder = drinkOrder & 0x7F;  // 0111 1111

  //note: 0x
  return drinkOrder; //probably 1, 2, or 3
}


/*receivedMsgWrapper(): wrapper for receiving messages over
   the CAN network.
*/
void receivedMsgWrapper() {
  //put one of these in -every- sketch for an arduino with a CAN chip.
  //we have to do it this way because there are some issues with calling
  //a function of an object that may or may not exist.
  // Serial.println("try receive");
  // digitalWrite(49,HIGH);
  if (!sendRunning) {
    scl.receivedMsg();
  }
  /*int err = scl.checkErr();
    if (err == 0){
    digitalWrite(49,LOW);
    } else {
    digitalWrite(49,HIGH);
    }
  */
  //digitalWrite(49,LOW);
  //sendRunning = false;
}



