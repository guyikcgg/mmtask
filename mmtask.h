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
*******************************************************************************/


// Exit states
#define TASK_EXIT_DONE       0
#define TASK_EXIT_RUNNING   -1
#define TASK_EXIT_TIMEOUT   -2
#define TASK_EXIT_WAITING   -3

// A helper macro
#define M_HELPER(token)  __##token##__+1
#define M M_HELPER(COUNTER)


//Include this line in your code (where `counter` should be a real timer, either
//a variable or a function)
//#define TIME_COUNTER counter

#ifndef TIME_COUNTER
#warning "mmtask: TIME_COUNTER is not defined. It should point to a real timer!"
unsigned mmtask_default_counter() {
    static unsigned counter = 0;
    return ++counter;
}
#define TIME_COUNTER mmtask_default_counter()
#endif

/* SET_RESOURCE: defines a specified quantity of a limited resource.
        Must be used in the global scope. */
#define SET_RESOURCE(resource_name, quantity) unsigned resource_name = quantity;

/* INCLUDE_RESOURCE: includes a resource defined in other file.
        Must be used in the global scope or in headers files. */
#define INCLUDE_RESOURCE(resource_name) extern unsigned resource_name;

/* TASK_STATE: separates the current state from the next one,
        without yielding to other tasks. */
#define TASK_STATE \
  time_init = TIME_COUNTER; \
  restartable_stage = M; \
  case M-1: \
  do {} while(0);

/* TASK_INIT: Initializes the task, going into its initial state.
        Must be the first macro of the task, just after declaration of local variables.*/
#define TASK_INIT \
  static long unsigned time_init; \
  static int restartable_stage = 0; \
  switch(restartable_stage) { \
  case 0: \
  TASK_STATE

/* TASK_YIELD: stop running this task to yield to other tasks. This defines
        a new state after the current one. */
#define TASK_YIELD \
  restartable_stage = M; \
  return TASK_EXIT_RUNNING; \
  time_init = TIME_COUNTER; \
  case M-1: \
  do {} while(0);

/* TASK_WAIT_RESOURCE: get n resources (if available) and define a new
        state. If the resource is not available, do nothing (wait for it) */
#define TASK_WAIT_RESOURCE(resource, n) \
  TASK_STATE \
  if (!(resource>=n)) return TASK_EXIT_WAITING; \
  (resource)--; \
  TASK_STATE

/* TASK_WAIT_RESOURCE_TIMEOUT: wait for n resources to be available,
          unless a certain timeout is reached */
#define TASK_WAIT_RESOURCE_TIMEOUT(resource, n, timeout, handler) \
  TASK_STATE \
  TASK_TIMEOUT(timeout, handler) \
  if (!(resource>=n)) return TASK_EXIT_WAITING; \
  (resource)--; \
  TASK_STATE

/* TASK_FREE_RESOURCE: free the resource previously gotten and define
        a new state*/
#define TASK_FREE_RESOURCE(resource, n) \
  (resource) += n; \
  TASK_STATE

/* TASK_YIELD_MINVT: yield to other tasks but don't go to the next state until
        a minimum visit time (minvt) has elapsed since the current state
        was reached */
#define TASK_YIELD_MINVT(minvt) \
  restartable_stage = M; \
  case M-1: \
  if (TIME_COUNTER-time_init<minvt) return TASK_EXIT_RUNNING; \
  TASK_STATE

/* TASK_REPEAT_WHILE: repeat the current state while the condition is true.
        After that, enter a new state. */
#define TASK_REPEAT_WHILE(condition) \
  if (!(condition)) TASK_YIELD

/* TASK_TIMEOUT: define what to do if a state has used too much time.
        This should be used at the beginning of a state. */
#define TASK_TIMEOUT(timeout, handler) \
  if (TIME_COUNTER-time_init>=timeout) goto handler;

/* TASK_RETURN_TIMEOUT: prepare the task to be re-initialized and return
        the corresponding error code */
#define TASK_RETURN_TIMEOUT \
  restartable_stage = 0; \
  return TASK_EXIT_TIMEOUT;

/* TASK_END: defines the end of the task. Anything below this sentences will
        be reached normally (unless a label is used). */
#define TASK_END \
  do {} while(0);} \
  restartable_stage = 0; \
  return TASK_EXIT_DONE;
