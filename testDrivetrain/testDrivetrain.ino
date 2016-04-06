//testDrivetrain: test the drivetrain.



int ultrasoundPins[5][2] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}};
//{{trig1, echo1}, {trig2, echo2}, etc.}
//order: fc, fl, fr, bc, br
int encoderPins[4][2] = {{11, 12}, {13, 14}, {15, 16}, {17, 18}};
//{{A1,B1},{A2,B2}, etc.}
//order: f, l, r, b
int motorPins[4][3] = {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}, {28, 29, 30}};
//{{INA1, INB1, PWM1}, {INA2, INB2, PWN2}, etc.}
//order: f, l, r, b


int motorSpeed = 100; // range: 0 to 255


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  char cmd = Serial.read();
  //Serial.println(cmd);
  switch (cmd) {
    case 'w': {
        //forward
        forward(motorSpeed);
        delay(500);
        stopMoving();
        break;
      }
    case 'q': {
        // rotate left
        turn(-motorSpeed);
        delay(500);
        stopMoving();
        break;
      }
    case 'a': {
        //strafe left
        strafe(-motorSpeed);
        delay(500);
        stopMoving();
        break;
      }
    case 'e': {
        //rotate right
        turn(motorSpeed);
        delay(500);
        stopMoving();
        break;
      }
    case 'd': {
        //strafe right
        strafe(motorSpeed);
        delay(500);
        stopMoving();
        break;
      }
    case 's': {
        //back
        back(motorSpeed);
        delay(500);
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
