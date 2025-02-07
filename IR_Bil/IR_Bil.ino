
#include "Servo.h"
#include <Arduino.h>
#define DECODE_NEC  // Includes Apple and Onkyo. To enable all protocols , just comment/disable this line.
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>  // include the library

// creating objects
Servo turnServo;

//creating constants
const int forwardRelay = 8;
const int backwardRelay = 9;

//creating variables
bool direction = 1;
int turnDegree = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;  // Wait for Serial to become available. Is optimized away for some cores.

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

  pinMode(forwardRelay, OUTPUT);
  pinMode(backwardRelay, OUTPUT);

  turnServo.attach(10);
  
}

void loop() {
  /*
  * Check if received data is available and if yes, try to decode it.
  */
  if (IrReceiver.decode()) {
    IRdecode();
  }
  turn();
}


/* 
* This function sets the servo to the right angle and makes the car turn.
* Parameters: void
* Return: void
*/
void turn(){
  int servoDegree;
  if (direction) {
    servoDegree = 90 + turnDegree;
  } else {
    servoDegree = 90 - turnDegree;
  }
  // Serial.println(servoDegree);
  turnServo.write(servoDegree);

}
/*
* This function prints a summary of received data. If the data is an existing command the 
* command will be saved as the last command. 
* parameters: void
* Return: void
*/
void IRdecode() {

  /*
  * Print a summary of received data
  */
  if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
    Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
    // We have an unknown protocol here, we skip this section and start recieving new data
    IrReceiver.printIRResultRawFormatted(&Serial, true);
    IrReceiver.resume();  // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
  } else {
    IrReceiver.resume();  // Early enable receiving of the next IR frame
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
  }
  Serial.println();

  /*
  * Finally, check the received data and perform actions according to the received command
  */
  checkCommand();
}


/*
* This function checks what to do with the data from the IR-reciever. It sets direction
* and if/how it is going to turn
* Parameters: void
* Return: void
*/
void checkCommand(){
  if (IrReceiver.decodedIRData.command == 0x46) {
    // go forward
    digitalWrite(forwardRelay, HIGH);
    digitalWrite(backwardRelay, LOW);
  } else if (IrReceiver.decodedIRData.command == 0x15) {
    // go backwards
    digitalWrite(forwardRelay, LOW);
    digitalWrite(backwardRelay, HIGH);
  } else if (IrReceiver.decodedIRData.command == 0x40) {
    // Stop
    digitalWrite(forwardRelay, LOW);
    digitalWrite(backwardRelay, LOW);
  } else if (IrReceiver.decodedIRData.command == 0x43) {
    // turn right
    direction = 1;
  } else if (IrReceiver.decodedIRData.command == 0x44) {
    // turn left
    direction = 0;
  } else if (IrReceiver.decodedIRData.command == 0x5A) {
    // press 9, turn 27 degrees
    turnDegree = 27;
  } else if (IrReceiver.decodedIRData.command == 0x1C) {
    // press 8, turn 24 degrees
    turnDegree = 24;
  } else if (IrReceiver.decodedIRData.command == 0x8) {
    // press 7, turn 21 degrees
    turnDegree = 21;
  } else if (IrReceiver.decodedIRData.command == 0x5E) {
    // press 6, turn 18 degrees
    turnDegree = 18;
  } else if (IrReceiver.decodedIRData.command == 0x18) {
    // press 5, turn 15 degrees
    turnDegree = 15;
  } else if (IrReceiver.decodedIRData.command == 0xC) {
    // press 4, turn 12 degrees
    turnDegree = 12;
  } else if (IrReceiver.decodedIRData.command == 0xD) {
    // press 3, turn 9 degrees
    turnDegree = 9;
  } else if (IrReceiver.decodedIRData.command == 0x19) {
    // press 2, turn 6 degrees
    turnDegree = 6;
  } else if (IrReceiver.decodedIRData.command == 0x16) {
    // press 1, turn 3 degrees
    turnDegree = 3;
  } else if (IrReceiver.decodedIRData.command == 0x52) {
    // press 0, dont turn
    turnDegree = 0;
  }
}