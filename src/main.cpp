#include <Arduino.h>
//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(10,11); //RX, TX

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Open serial communications and wait for port to open:
  //Serial.begin(9600); //serial port to computer
  //mySerial.begin(57600); //serial port to radio
  //Serial.println("Startup");

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  
  //if (mySerial.available())
    //Serial.write(mySerial.read());
  //if (Serial.available())
   // mySerial.write(Serial.read());


}