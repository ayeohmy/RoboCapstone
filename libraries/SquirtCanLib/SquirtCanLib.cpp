//squirtlib.c: the functions for everyone 

/*
 * SquirtCanLib.cpp - Library used to send Project Squirt messages over a CAN 
 * network using MCP2515 CAN chips connected over SPI to Arduino boards. This
 * library acts as a wrapper for the MCP_CAN Library for Arduino by coryjflower
 * (url: https://github.com/coryjfowler/MCP_CAN_lib)
 *
 * Created by Zachary Dawson 
 * Team <1% Robot
 * for 16-474 Project Squirt
 */

#include "Arduino.h"
#include "SquirtCanLib.h"
#include <mcp_can.h>

void SquirtCanLib::canSetup(int spiPins[]) {
  //dummy function for CAN setup
	//does nothing...
}

int SquirtCanLib::sendMsg(int msgHdr, char payload) {
  //dummy function for CAN library sendMsg
  return 0;

}

unsigned char SquirtCanLib::getMsg(int msgHdr) {
  //dummy function for CAN library getMsg
  return '0';
}
