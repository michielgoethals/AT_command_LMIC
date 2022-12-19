#include "readUartCommand.h"

#define BAUDRATE 19200
#define RX 7
#define TX 8
#define LED 13

ReadUartCommand reader(RX, TX); 

char * command = nullptr;

void setup(){
  pinMode(LED, OUTPUT);
  Serial.begin(19200);
  Serial.println("Startup");
  reader.begin(BAUDRATE);
}

void loop(){
  command = reader.getCommand();
  Serial.write(command);
  if (strcmp(command,"")!=0){
    reader.parseCommand(command);
  }
  if(digitalRead(LED)==HIGH){
    digitalWrite(LED,LOW);
  }else{
    digitalWrite(LED,HIGH);
  }
  delay(1000);
  delete[] command;
}
