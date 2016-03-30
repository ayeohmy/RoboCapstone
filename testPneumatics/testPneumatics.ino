//testPneumatics: test them pneumatics

//TODO: incremental testing code. all of it..... .


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

//HIGH on the pressure sensor means that the pressure is LOW.
//so if it's HIGH/1, then you need to turn it on!
bool runUltrasound = true;
int valvePins[] = {11, 12, 13};
int pressureSensorPin = 14;
int compressorPin = 15;
int ultrasoundPins[] = {16, 17}; //{trig, echo}

double maxDrinkDist = 11.0; //let's say when we're 4.0cm from the drink, we stop


void setup() {
  // put your setup code here, to run once:
  //if (runUltrasound) {
  pinMode(ultrasoundPins[0], OUTPUT);
  pinMode(ultrasoundPins[1], INPUT);
  //}

  pinMode(valvePins[0], OUTPUT);
  pinMode(valvePins[1], OUTPUT);
  pinMode(valvePins[2], OUTPUT);

  pinMode(pressureSensorPin, INPUT);
  pinMode(compressorPin, OUTPUT);
  Serial.begin(9600);

  Serial.println("enter a number (1,2, or 3) to open that valve, 0 to close all. a, s, or d to dispense ye a drink");
}

void loop() {
  // put your main code here, to run repeatedly:


  // if the thing is low pressure, turn on the compressor
  //otherwise don't
  volatile int needPressure = digitalRead(pressureSensorPin);
  //Serial.println(needPressure);
  if (needPressure == 1) {
    digitalWrite(compressorPin, HIGH);
    //  Serial.println("compressor ON");
  } else {
    digitalWrite(compressorPin, LOW);
    //  Serial.println("compressor OFF");
  }

  char valveNo = Serial.read();
  //Serial.println(valveNo);
  switch (valveNo) {
    case '0': {
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        Serial.println("close all");
        break;
      }
    case '1': {
        digitalWrite(valvePins[0], HIGH);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        Serial.println("open 1");
        break;
      }
    case '2': {
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], HIGH);
        digitalWrite(valvePins[2], LOW);
        Serial.println("open 2");
        break;
      }
    case '3': {
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], HIGH);
        Serial.println("open 3");
        break;
      }
    case 'a': {
        //dispense drink 1
        Serial.println("dispensing 1...");
        int drinkDist = getRange(ultrasoundPins);
        Serial.print("drink distance: ");
        Serial.println(drinkDist);
        while (drinkDist > maxDrinkDist) {
          digitalWrite(valvePins[0], HIGH);
          digitalWrite(valvePins[1], LOW);
          digitalWrite(valvePins[2], LOW);
          Serial.print("drink distance: ");
          Serial.println(drinkDist);
          delay(50);
          drinkDist = getRange(ultrasoundPins);
          Serial.print("drink distance: ");
          Serial.println(drinkDist);
        }
        Serial.println("done dispensing 1!");
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        break;
      }
    case 's': {
        //dispense drink 2
        Serial.println("dispensing 2...");
        int drinkDist = getRange(ultrasoundPins);
        while (drinkDist > maxDrinkDist) {
          digitalWrite(valvePins[0], LOW);
          digitalWrite(valvePins[1], HIGH);
          digitalWrite(valvePins[2], LOW);
          Serial.print("drink distance: ");
          Serial.println(drinkDist);
          delay(50);
          drinkDist = getRange(ultrasoundPins);
          Serial.print("drink distance: ");
          Serial.println(drinkDist);
        }
        Serial.println("done dispensing 2!");
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        break;
      }
    case 'd': {
        //dispense drink 3
        Serial.println("dispensing 3...");
        int drinkDist = getRange(ultrasoundPins);
        while (drinkDist > maxDrinkDist) {
          digitalWrite(valvePins[0], LOW);
          digitalWrite(valvePins[1], LOW);
          digitalWrite(valvePins[2], HIGH);
          Serial.print("drink distance: ");
          Serial.println(drinkDist);
          delay(50);
          drinkDist = getRange(ultrasoundPins);
          Serial.print("drink distance: ");
          Serial.println(drinkDist);
        }
        Serial.println("done dispensing 3!");
        digitalWrite(valvePins[0], LOW);
        digitalWrite(valvePins[1], LOW);
        digitalWrite(valvePins[2], LOW);
        break;
      }


    default:
      break;
  }
  delay(100); //10 Hz-ish is fine? how does delay() even work
}


double getRange(int pins[]) {
  int trigPin = pins[0];
  int echoPin = pins[1];
  // Serial.println(pins[0]);
  //  Serial.println(pins[1]);
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


