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
* This example demonstrates the use of mmtask with a very simple program.
* It has been used to check the functionalities of the provided library.
*
* The example has been designed to work on a GNU/Linux machine, but it should
* also work on other OS or in a naked microcontroller.
*
*******************************************************************************/

#include <stdio.h>
#include "../mmtask.h"

/* Prototypes of the used tasks */
int task1();
int task2();
int task1_1();
int task1_2();
int task2_1();
int task2_2();

/* TIME_COUNTER definition (normally, it would be a rtc register or function) */
long unsigned int iterative_counter = 100;
#define TIME_COUNTER iterative_counter

// Define every resource here
SET_RESOURCE(resource_1, 1);

/* main: execute task1 and task2 concurrently (in parallel) */
int main(int argc, char **argv) {
  char a[10] = {0};

  while(1) {
    iterative_counter += 10;
    task1();
    task2();
    *a = getchar();
    printf("%s\n", a);
  }
}

/* task1: execute task1_1 until it is done, then, execute task1_2 */
int task1() {
  int task1_1_status;

  TASK_BEGIN;

  task1_1_status = task1_1();

  TASK_REPEAT_WHILE(task1_1_status != TASK_EXIT_DONE);

  task1_2();

  TASK_END;
}

/* task2: execute task2_1 and task2_2 in parallel */
int task2() {
  TASK_BEGIN;

  task2_1();
  TASK_YIELD;

  task2_2();

  TASK_END;
}

/* task1_1: get the defined resource, repeat an action three times and
            get the resource back */
int task1_1() {
  static int i;

  TASK_BEGIN;
  // STATE_0:
  i = 0;

  TASK_WAIT_RESOURCE(resource_1);
  // STATE_1:

  printf("resource got by task1_1, %d\n", resource_1);

  TASK_STATE;
  //STATE_2:

  printf("i=%d\n", i++);

  TASK_TIMEOUT(50, GO_AHEAD);

  TASK_REPEAT_WHILE(i<10);
  // STATE_3:

  GO_AHEAD:
  TASK_FREE_RESOURCE(resource_1);
  // STATE_4:

  printf("resource back (task1_1), %d\n", resource_1);

  TASK_YIELD;
  // STATE_5:

  printf("Ending task1_1\n");

  TASK_END;
}

/* task1_2: print a simple message */
int task1_2() {
  printf("task1_2, %d\n", resource_1);
}

/* task2_1: get the defined resource, make sure a certain time has passed, then
            get the resource back */
int task2_1 () {
  TASK_BEGIN;
  // STATE_0:

  TASK_WAIT_RESOURCE(resource_1);
  // STATE_1:

  printf("resource got by task2_1, %d\n", resource_1);

  TASK_YIELD_MINVT(40);
  // STATE_2:

  TASK_FREE_RESOURCE(resource_1);
  // STATE_3:

  printf("resource back (task2_1), %d\n", resource_1);

  TASK_END;
}

/* task1_2: print a simple message */
int task2_2() {
  printf("task2_2, %d\n", resource_1);
}
