/*******************************************************************************
* MIT License
*
* Copyright (c) 2016 Cristian González Guerrero
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
********************************************************************************
* DESCRIPTION
*
* This example demonstrates the use of mmtask with a very simple sketch.
*
* The example has been designed to work on Arduino UNO, but it should also work 
* on other OS or in a naked microcontroller.
*
*******************************************************************************/

#include "mmtask.h"

/* Prototypes of the used tasks */
int task_PrintInputPin();
int task_Blink();
int task_PrintReceived();

/* TIME_COUNTER definition */
#define TIME_COUNTER millis()

/* Definition of the resources */
SET_RESOURCE(r_serial, 1);

/* Global variables */
const int ledPin    = 13;     // select the pin for the LED
const int inputPin  = A0;     // select the pin for the Analog Input

/* SETUP */
void setup() {
  // Initialize serial
  Serial.begin(9600);
  
  // Initialize digital pin 13 as an output
  pinMode(ledPin, OUTPUT);
}

/* LOOP */
void loop() {
  // Execute the tasks in parallel

  task_PrintInputPin();
  task_Blink();
  task_PrintReceived();

}

/* task_PrintInput: read 'inputPin' and print the value (every 200ms) */
int task_PrintInputPin(){
  static int sensorValue;
  
  TASK_BEGIN;
  STATE_0:

  // Take the resource in order not to influence task_PrintReceived output
  TASK_WAIT_RESOURCE(r_serial, 1);
  STATE_1:
  
    sensorValue = analogRead(inputPin);  
    Serial.println(sensorValue, DEC);

  // Always free the resources as soon as possible
  TASK_FREE_RESOURCE(r_serial, 1);
  STATE_2:
  
  TASK_YIELD_MINVT(200);
  STATE_3:

  TASK_END;
}

/* task_Blink: toggle 'ledPin' every 500ms */
int task_Blink() {
  TASK_BEGIN;
  STATE_0:
  
    digitalWrite(ledPin, HIGH);

  TASK_YIELD_MINVT(500);
  STATE_1:

    digitalWrite(ledPin, LOW);

  TASK_YIELD_MINVT(500);
  STATE_2:

  
  TASK_END;
}

/* task_PrintReceived: print received data */
int task_PrintReceived() {
  char incomingByte;
  
  TASK_BEGIN;
  STATE_0:

    // Select a timeout for this state
    TASK_TIMEOUT(10000, STATE_TIMEOUT);
  
  TASK_REPEAT_WHILE(!Serial.available() > 0);
  STATE_1:

  // Take the resource in order not to influence task_PrintInput output
  TASK_WAIT_RESOURCE(r_serial, 1);
  STATE_2:
  
    incomingByte = Serial.read();
    Serial.print(incomingByte);
  
  TASK_REPEAT_WHILE(Serial.available() > 0);
  STATE_3:

    Serial.println("");
  
  // Always free the resources as soon as possible
  TASK_FREE_RESOURCE(r_serial, 1);
  STATE_4:

    // Skip STATE_TIMEOUT (and following)
    goto STATE_END;

  STATE_TIMEOUT:
  TASK_WAIT_RESOURCE(r_serial, 1);
    Serial.println("No data received in the last 10s");
  TASK_FREE_RESOURCE(r_serial, 1);
  
  STATE_END:
  TASK_END;
}

