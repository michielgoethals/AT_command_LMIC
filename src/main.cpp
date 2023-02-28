#include "readUartCommand.h"

#define BAUDRATE 19200

char * command = nullptr;
ReadUartCommand reader; 

void setup(){
  reader.begin(BAUDRATE);
}

void loop(){
  command = reader.getCommand();
  if (strcmp(command,"")!=0){
    reader.parseCommand(command);
  }

  delay(1000);

  delete[] command;
}
