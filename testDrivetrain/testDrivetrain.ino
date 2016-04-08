//testDrivetrain: test the drivetrain.
//you can drive for a short time in any direction, 
//or drive continuously in any direction until an ultrasound
//sees something. 
//currently not listening to the encoders. probably should do that at some point...


/* COMMANDS:
 *  To stop at any time: press space and then enter 
 *  
 *  To drive for a short time: 
 * w,s: front, back
 * a,d: strafe left, strafe right
 * q,e: turn left, turn right 
 * 
 * To drive continuously: 
 * i,k: front, back
 * j,l: strafe left, strafe right
 * u,o: turn left, turn right 
 * 
 * To check ultrasound status: m
 * 
 * To change speed: 
 * 1: v. slow
 * 2: slow
 * 3: medium
 * 4: fast
 * 5: v. fast 
 * 
 */

int ultrasoundPins[5][2] = {{23,24}, {25,26}, {27,28}, {29,30}, {31,32}};
//{{trig1, echo1}, {trig2, echo2}, etc.}
//order: fc, fl, fr, bc, br
int encoderPins[4][2] = {{15,16}, {17,18}, {19,20}, {21,22}};
//{{A1,B1},{A2,B2}, etc.}
//order: f, l, r, b
int motorPins[4][3] = {{2,7,3}, {4,8,9}, {5,12,10},{6,13,11}};
//{{INA1, INB1, PWM1}, {INA2, INB2, PWN2}, etc.}
//order: f, l, r, b


int motorSpeed = 100; // range: 0 to 255
int motorTime = 5000; //ms to drive

void setup() {
  // put your setup code here, to run once:
   for (int i = 0; i < 5; i++) {
    pinMode(ultrasoundPins[i][0], OUTPUT);
    pinMode(ultrasoundPins[i][1], INPUT);
  }
  //motor stuff setup
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i][0], OUTPUT);
    pinMode(motorPins[i][1], OUTPUT);
    pinMode(motorPins[i][2], OUTPUT);
 }
Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:

  char cmd = Serial.read();
  //Serial.println(cmd);
  switch (cmd) {
    case 'w': {
      Serial.println("forward");
        //forward
        forward(motorSpeed);
        delay(motorTime);
        stopMoving();
        break;
      }
    case 'q': {
        Serial.println("turn left");
        // rotate left
        turn(-motorSpeed);
        delay(motorTime);
        stopMoving();
        break;
      }
    case 'a': {
        Serial.println("left");
        //strafe left
        strafe(-motorSpeed);
        delay(motorTime);
        stopMoving();
        break;
      }
    case 'e': {
      Serial.println("turn right");
        //rotate right
        turn(motorSpeed);
        delay(motorTime);
        stopMoving();
        break;
      }
    case 'd': {
      Serial.println("right"); 
        //strafe right
        strafe(motorSpeed);
        delay(motorTime);
        stopMoving();
        break;
      }
    case 's': {
      Serial.println("back"); 
        //back
        back(motorSpeed);
        delay(motorTime);
        stopMoving();
        break;
      }

    //CONTINUOUS GOERS

    case 'i': {
        //forward
        runContinuously('w');
        break;
      }
    case 'u': {
        // rotate left
        runContinuously('q');
        break;
      }
    case 'j': {
        //strafe left
        runContinuously('a');
        break;
      }
    case 'o': {
        //rotate right
        runContinuously('e');
        break;
      }
    case 'l': {
        //strafe right
        runContinuously('d');
        break;
      }
    case 'k': {
        //back
        runContinuously('s');
        break;
      }

    //SPEED SET


    case '1': {
        //slowest
        motorSpeed = 50;
        break;
      }
    case '2': {
        motorSpeed = 100;
        break;
      }
    case '3': {
        motorSpeed = 150;
        break;
      }
    case '4': {
        motorSpeed = 200;
        break;
      }
    case '5': {
        motorSpeed = 250;
        break;
      }

    //STATUS
    case 'm': {
        //check ultrasounds
        double dist = getRange(ultrasoundPins[0]);
        Serial.println("front center:");
        Serial.println(dist);
        dist = getRange(ultrasoundPins[1]);
        Serial.print("front left");
        Serial.println(dist);
        dist = getRange(ultrasoundPins[2]);
        Serial.print("front right");
        Serial.println(dist);
        dist = getRange(ultrasoundPins[3]);
        Serial.print("back left:");
        Serial.println(dist);
        dist = getRange(ultrasoundPins[4]);
        Serial.print("back right:");
        Serial.println(dist);
        break;
      }
    default:
      break;
  }
}

//SO MANY HELPERS


void forward(int spd) {
  setMotor(motorPins[1], spd);
  setMotor(motorPins[2], spd);

}

void back(int spd) {
  forward(-spd);
}

void strafe(int spd) {
  //strafe right or left. positive == right, negative == left
  setMotor(motorPins[0], spd);
  setMotor(motorPins[3], spd);
}

void turn (int spd) {
  //currently rotating with the front and back wheels only. is this okay?
  setMotor(motorPins[0], spd);
  setMotor(motorPins[3], spd);
}

void stopMoving() {
  setMotor(motorPins[0], 0);
  setMotor(motorPins[1], 0);
  setMotor(motorPins[2], 0);
  setMotor(motorPins[3], 0);

}

int runContinuously(char dir) {
  //runs the specified motor (motor) continuously
  //in the specified direction (dir) until we hit the
  //corresponding limit switch.
  //

  //set up stopping sensor(s)
  int pins1[2];
  int pins2[2];
  switch (dir) {
    case 'w': {
        //forward
        pins1[0] = ultrasoundPins[0][0];
        pins1[1] = ultrasoundPins[0][1];
        pins2[0] = 0; //just look in front
        pins2[1] = 0;
        break;
      }
    case 'q': {
        // rotate left
        pins1[0] = ultrasoundPins[1][0];
        pins1[1] = ultrasoundPins[1][1];
        pins2[0] = ultrasoundPins[4][0];
        pins2[1] = ultrasoundPins[4][1];
        break;
      }
    case 'a': {
        //strafe left
        pins1[0] = ultrasoundPins[1][0];
        pins1[1] = ultrasoundPins[1][1];
        pins2[0] = ultrasoundPins[3][0];
        pins2[1] = ultrasoundPins[3][1];
        break;
      }
    case 'e': {
        pins1[0] = ultrasoundPins[2][0];
        pins1[1] = ultrasoundPins[2][1];
        pins2[0] = ultrasoundPins[3][0];
        pins2[1] = ultrasoundPins[3][1];
        break;
      }
    case 'd': {
        //right
        pins1[0] = ultrasoundPins[2][0];
        pins1[1] = ultrasoundPins[2][1];
        pins2[0] = ultrasoundPins[4][0];
        pins2[1] = ultrasoundPins[4][1];
        break;
      }
    case 's': {
        //back
        pins1[0] = ultrasoundPins[3][0];
        pins1[1] = ultrasoundPins[3][1];
        pins2[0] = ultrasoundPins[4][0];
        pins2[1] = ultrasoundPins[4][1];
        break;
      }
    default:
      break;
  }

  double  range1 = getRange(pins1);
  double range2 = getRange(pins2);
  int degsSoFar = 0;
  while (range1 > 5 && range2 > 5) {

    switch (dir) {
      case 'w': {
          //forward
          forward(motorSpeed);
          break;
        }
      case 'q': {
          // rotate left
          turn(-motorSpeed);
          break;
        }
      case 'a': {
          //strafe left
          strafe(-motorSpeed);
          break;
        }
      case 'e': {
          //rotate right
          turn(motorSpeed);
          break;
        }
      case 'd': {
          //strafe right
          strafe(motorSpeed);
          break;
        }
      case 's': {
          //back
          back(motorSpeed);
          break;
        }
      default:
        break;
    }
    range1 = getRange(pins1);
    range2 = getRange(pins2);

    char cmd = Serial.read();
    if (cmd == ' ') {
      //STOP....
      Serial.println("STOPPIN");
      range1 = 0;
      range2 = 0;
    }
  }
  stopMoving();

}



void setMotor(int pins[], int spd) {
  //input format: speed (+- int), pins ({inA,inB,pwm})
  if (spd >= 0) {
    //go forward. for now, assuming this is counterclockwise.
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



double getRange(int pins[]) {
  //gets the range measurement from an ultrasound sensor.
  //pins[] is a two-element array {t,e},
  //where t is the pin no. for the TRIG pin and e is the pin no.
  //of the ECHO pin for this sensor.
  //this pin pass-in format is needed for ranging from all four
  //of the drivetrain sensors in the drivetrain sketch.

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
