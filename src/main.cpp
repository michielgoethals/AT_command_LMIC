#include "readUartCommand.h"

#define BAUDRATE 19200
#define RX 7
#define TX 8
#define LED 13

ReadUartCommand reader(RX, TX); 

char * command = nullptr;

void setup(){
  Serial.begin(9600);
  Serial.println("Startup");
  reader.begin(BAUDRATE);
}

void loop(){
  command = reader.getCommand();
  Serial.write(command);
  if (strcmp(command,"")!=0){
    reader.parseCommand(command);
  }

  delay(1000);

  delete[] command;
}
