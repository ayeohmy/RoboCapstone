#include <SquirtCanLib.h>

SquirtCanLib scl;

int slavePin = 10;
int interruptPin = 2;

void setup() {
  Serial.begin(9600);
  Serial.println("setup0");
  Serial.flush();
  scl.canSetup(slavePin);
  Serial.println("setup1");
  Serial.flush();
  pinMode(interruptPin, INPUT);
  Serial.println("setup2");
  attachInterrupt(digitalPinToInterrupt(interruptPin), receivedMsgWrapper, LOW);
  Serial.println("setup3");
}

void loop() {
  unsigned char got = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER);
  Serial.println(got, HEX);
  got = scl.getMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS);
  Serial.print("Status: ");
  Serial.println(got, HEX);
  delay(3000);
}

void receivedMsgWrapper() {
  scl.receivedMsg();
}
