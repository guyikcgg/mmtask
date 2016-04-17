#include <stdio.h>
#include "mmtask.h"

int task1();
int task2();
int task1_1();
int task1_2();
int task2_1();
int task2_2();

long unsigned int iterative_counter = 100;

#define TIME_COUNTER iterative_counter

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

int resource_1 = 1;

int task1() {
  int task1_1_status;

  TASK_BEGIN;

  task1_1_status = task1_1();

  TASK_REPEAT_WHILE(task1_1_status != TASK_EXIT_DONE);

  task1_2();

  TASK_END;
}

int task2() {
  TASK_BEGIN;

  task2_1();
  TASK_YIELD;

  task2_2();

  TASK_END;
}

int task1_1() {
  static int i;

  TASK_BEGIN;
  i = 0;

  TASK_WAIT_RESOURCE(&resource_1);
  printf("resource got by task1_1, %d\n", resource_1);

  printf("i=%d\n", i++);

  TASK_REPEAT_WHILE(i<3);

  TASK_FREE_RESOURCE(&resource_1);
  printf("resource back (task1_1), %d\n", resource_1);

  TASK_YIELD;

  printf("Ending task1_1\n");

  TASK_END;
}

int task1_2() {
  printf("task1_2, %d\n", resource_1);
}

int task2_1 () {
  TASK_BEGIN;

  TASK_WAIT_RESOURCE(&resource_1);
  printf("resource got by task2_1, %d\n", resource_1);

  TASK_YIELD_MINVT(40);
  //TASK_YIELD;

  TASK_FREE_RESOURCE(&resource_1);
  printf("resource back (task2_1), %d\n", resource_1);

  TASK_END;
}

int task2_2() {
  printf("task2_2, %d\n", resource_1);
}
