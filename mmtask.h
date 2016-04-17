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


// Exit statuses
#define TASK_EXIT_DONE       0
#define TASK_EXIT_RUNNING   -1
#define TASK_EXIT_TIMEOUT   -2
#define TASK_EXIT_WAITING   -3

#define M_HELPER(token)  __##token##__+1
#define M M_HELPER(COUNTER)


//Include this line in your code (where counter should be a real-time counter)
//#define TIME_COUNTER counter

#define SET_RESOURCE(resource_name, quantity) unsigned resource_name = quantity;

#define TASK_STATE \
  elapsed_time = TIME_COUNTER; \
  restartable_stage = M; case M-1:

#define TASK_BEGIN \
  static long unsigned elapsed_time; \
  static int restartable_stage = 0; \
  switch(restartable_stage) { case 0: \
  TASK_STATE

#define TASK_YIELD \
  restartable_stage = M; return TASK_EXIT_RUNNING; \
  elapsed_time = TIME_COUNTER; \
  case M-1:

#define TASK_WAIT_RESOURCE(resource) \
  restartable_stage = M; case M-1: \
  if (!resource) return TASK_EXIT_WAITING; \
  (resource)--; \
  TASK_STATE

#define TASK_FREE_RESOURCE(resource) \
  (resource)++; \
  TASK_STATE

#define TASK_YIELD_MINVT(ms) \
  restartable_stage = M; case M-1: \
  if (TIME_COUNTER-elapsed_time<ms) { return TASK_EXIT_RUNNING; } \
  TASK_STATE

#define TASK_REPEAT_WHILE(condition) \
  if (!(condition)) TASK_YIELD

#define TASK_TIMEOUT(timeout, handler) \
  if (TIME_COUNTER-elapsed_time>timeout) goto handler;

#define TASK_END \
  } restartable_stage = 0; return TASK_EXIT_DONE;
