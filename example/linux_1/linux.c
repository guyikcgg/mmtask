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
#include <mmtask.h>


/* Prototypes of the used tasks */
int task1();
int task2();
int task1_1();
int task1_2();
int task2_1();
int task2_2();

/* TIME_COUNTER definition (normally, it would be a rtc register or function) */
long unsigned int iterative_counter = 0;
#define TIME_COUNTER iterative_counter

/* Definition of the resources */
SET_RESOURCE(resource_1, 1);

/* main: execute task1 and task2 concurrently (in parallel) */
int main(int argc, char **argv) {
  char a;

  while(1) {
    iterative_counter += 1;

    printf("\n\n ---- iterative_counter = %lu ---- \n\n", iterative_counter);

    task1();
    task2();

    // PAUSE
    a = getchar();
  }
}

/* task1: execute task1_1 until it is done, then, execute task1_2 */
int task1() {
  int task1_1_status;

  TASK_BEGIN;
  STATE_0:

  task1_1_status = task1_1();

  TASK_REPEAT_WHILE(task1_1_status != TASK_EXIT_DONE);
  STATE_1:

  task1_2();

  TASK_END;
}

/* task2: execute task2_1 and task2_2 in parallel */
int task2() {
  TASK_BEGIN;
  STATE_0:

  task2_1();

  TASK_STATE;
  STATE_1:

  task2_2();

  TASK_END;
}

/* task1_1: get the defined resource, repeat an action three times and
            get the resource back */
int task1_1() {
  static int i;

  printf("\nExecuting task1_1.\n");

  TASK_BEGIN;
  STATE_0:

  i = 0;
  TASK_WAIT_RESOURCE(resource_1, 1);

  STATE_1:

  printf("  STATE_1:  \n");
  printf("    -> 1x 'resource_1' taken ('resource_1' remaining: %d)\n", resource_1);

  TASK_STATE;
  STATE_2:
  printf("  STATE_2:  \n");

  TASK_TIMEOUT(5, HANDLE_TIMEOUT);

  i += 1;
  printf("    -> i=%d\n", i);

  TASK_REPEAT_WHILE(i<10);
  STATE_3:
  printf("  STATE_3:  \n");

  TASK_FREE_RESOURCE(resource_1, 1);

  STATE_4:
  printf("  STATE_4:  \n");

  printf("    -> 1x 'resource_1' released ('resource_1' remaining: %d)\n", resource_1);

  TASK_YIELD;
  STATE_5:
  printf("  STATE_5:  \n");

  printf("    -> Ending task1_1\n");

  TASK_END;

  HANDLE_TIMEOUT:
  printf("  HANDLE_TIMEOUT:  \n");

  printf("    -> Timeout occurred in task1_1!\n");

  goto STATE_3;
}

/* task1_2: print a simple message */
int task1_2() {
  printf("\nExecuting task1_2.\n");
}

/* task2_1: get the defined resource, make sure a certain time has passed, then
            get the resource back */
int task2_1 () {

  printf("\nExecuting task2_1.\n");

  TASK_BEGIN;
  STATE_0:
  printf("  STATE_0:  \n");

  TASK_WAIT_RESOURCE_TIMEOUT(resource_1, 1, 2, HANDLE_TIMEOUT);
  STATE_1:
  printf("  STATE_1:  \n");

  printf("    -> 1x 'resource_1' taken ('resource_1' remaining: %d)\n", resource_1);

  TASK_YIELD_MINVT(4);
  STATE_2:
  printf("  STATE_2:  \n");

  TASK_FREE_RESOURCE(resource_1, 1);
  STATE_3:
  printf("  STATE_3:  \n");

  printf("    -> 1x 'resource_1' released ('resource_1' remaining: %d)\n", resource_1);

  TASK_END;

  HANDLE_TIMEOUT:
  printf("  HANDLE_TIMEOUT:  \n");
  printf("    -> Timeout occurred in task2_1!\n");
  TASK_RETURN_TIMEOUT;
}

/* task1_2: print a simple message */
int task2_2() {
  printf("\nExecuting task2_2.\n");
}
