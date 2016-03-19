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

int valvePins[] = {6, 5, 7};
int pressureSensorPin = 3;
int compressorPin = 4;


void setup() {
  // put your setup code here, to run once:
  pinMode(valvePins[0], OUTPUT);
  pinMode(valvePins[1], OUTPUT);
  pinMode(valvePins[2], OUTPUT);0
  
  pinMode(pressureSensorPin, INPUT);
  pinMode(compressorPin, OUTPUT);
  Serial.begin(9600); 
  
  Serial.println("enter a number (1,2, or 3) to open that valve, 0 to close all.");
}

void loop() {
  // put your main code here, to run repeatedly:


  // if the thing is low pressure, turn on the compressor
  //otherwise don't
  volatile int needPressure = digitalRead(pressureSensorPin);
  Serial.println(needPressure);
  if (needPressure == 1) {
    digitalWrite(compressorPin, HIGH);
    Serial.println("compressor ON");
  } else {
    digitalWrite(compressorPin, LOW);
    Serial.println("compressor OFF"); 
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

    default:
      break;
  }
  delay(100); //10 Hz-ish is fine? how does delay() even work
}
