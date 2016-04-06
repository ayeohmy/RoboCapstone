//testPneumatics: test them pneumatics


//notes about hardware: 
//HIGH on the pressure sensor means that the pressure is LOW.
//so if it's HIGH/1, then you need to turn it on!
bool runUltrasound = true;
int valvePins[] = {11, 12, 13};
int pressureSensorPin = 14;
int compressorPin = 15;
int ultrasoundPins[] = {16, 17}; //{trig, echo}

double maxDrinkDist = 11.0; //let's say when we're 4.0cm from the drink, we stop


void setup() {
  // set up ultrasound pins
  if (runUltrasound) {
    pinMode(ultrasoundPins[0], OUTPUT);
    pinMode(ultrasoundPins[1], INPUT);
  }

  //set up vavle pins
  pinMode(valvePins[0], OUTPUT);
  pinMode(valvePins[1], OUTPUT);
  pinMode(valvePins[2], OUTPUT);

  //set up pressure sensor and compressor
  pinMode(pressureSensorPin, INPUT);
  pinMode(compressorPin, OUTPUT);
  Serial.begin(9600);

  Serial.println("enter a number (1,2, or 3) to open that valve, 0 to close all. a, s, or d to dispense ye a drink");
}

void loop() {
  // put your main code here, to run repeatedly:

  //keep the system pressurized, always!
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
        closeValves();
        Serial.println("close all");
        break;
      }
    case '1': {
        openValve(1);
        Serial.println("open 1");
        break;
      }
    case '2': {
        openValve(2);
        Serial.println("open 2");
        break;
      }
    case '3': {
        openValve(3);
        Serial.println("open 3");
        break;
      }
    case 'a': {
        //dispense drink 1
        dispenseDrink(1);
        break;
      }
    case 's': {
        //dispense drink 2
        dispenseDrink(2);
        break;
      }
    case 'd': {
        //dispense drink 3
        dispenseDrink(3);
        break;
      }

    default:
      break;
  }
  delay(100); 
}

void dispenseDrink(int drinkNo) {
  //dispenses the specified drink (drinkNo == 1, 2, or 3).
  //for any other value of drinkNo, does nothing.
  if (drinkNo < 1 || drinkNo > 3) return;


  //Serial.print("dispensing ");
  //Serial.println(drinkNo);
  int drinkDist = getRange(ultrasoundPins);

  //while the drink's height is below the threshold height,
  //keep dispensing
  while (drinkDist > maxDrinkDist) {
    openValve(drinkNo);

    delay(50); //so we don't loop too tightly
    drinkDist = getRange(ultrasoundPins);
    //Serial.print("drink distance: ");
    //Serial.println(drinkDist);
  }

  //stop dispensing
  //Serial.print("done dispensing");
  //Serial.println(drinkNo);
  closeValves();
}

void openValve(int valveNo) {
  //opens the specified valve (valveNo == 1, 2, or 3).
  //for any other value of valveNo, shuts all valves.
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

void closeValves() {
  //closes all valves.
  openValve(0);

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


