#include <SquirtCanLib.h>

SquirtCanLib scl;

int slavePin = 10;
int interruptPin = 2;
INT8U count = 0;
INT8U count2 = 255;

void setup() {
  Serial.begin(9600);
  Serial.println("setup0");
  Serial.flush();
  scl.canSetup(slavePin);
  Serial.println("setup3");
}

void loop() {
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_DRINK_ORDER, count);
  scl.sendMsg(SquirtCanLib::CAN_MSG_HDR_PREP_STATUS, count2);
  Serial.println("Sent");
  delay(1000);
  count++;
  count2--;
}
