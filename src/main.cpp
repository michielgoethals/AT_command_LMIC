#include "readUartCommand.h"

#define BAUDRATE 19200

osjob_t initjob;
osjob_t readAt;
ReadUartCommand reader; 

char * command = nullptr;

static void initfunc(osjob_t* j){
  //initializes serial communication and mac parameters
  reader.begin();
  //init done
}

static void readAtfunc(osjob_t* j){
  //read command from serial port
  command = reader.getCommand();
  //parse command
  reader.parseCommand(command);
}


void setup(){
  //initialize run-time environment
  os_init();
  //
  os_setCallback(&initjob, initfunc);
}

void loop(){
  //schedule a read command if there is data available
  if(Serial.available()>0){
    //give time to fill buffer with full command and then read it
    //since we dont know the length of the command
    delay(100);
    os_setCallback(&readAt, readAtfunc);
  }
  //execute scheduled jobs and events
  os_runloop_once();
}


