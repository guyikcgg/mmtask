// Exit statuses
#define TASK_EXIT_DONE       0
#define TASK_EXIT_RUNNING   -1
#define TASK_EXIT_TIMEOUT   -2
#define TASK_EXIT_WAITING   -3

#define M_HELPER(token)  __##token##__+1
#define M M_HELPER(COUNTER)

//Include this line in your code (where counter should be a real-time counter)
//#define TIME_COUNTER counter

#define TASK_BEGIN \
  static long unsigned elapsed_time; \
  static int restartable_stage = 0; \
  switch(restartable_stage) { case 0: \
  elapsed_time = TIME_COUNTER; \
  restartable_stage = M; case M-1:

#define TASK_YIELD \
  restartable_stage = M; return TASK_EXIT_RUNNING; \
  elapsed_time = TIME_COUNTER; \
  case M-1:

#define TASK_WAIT_RESOURCE(resource) \
  restartable_stage = M; case M-1: \
  if (!*resource) return TASK_EXIT_WAITING; \
  (*resource)--; \
  elapsed_time = TIME_COUNTER; \
  restartable_stage = M; case M-1:

#define TASK_FREE_RESOURCE(resource) \
  (*resource)++; \
  elapsed_time = TIME_COUNTER; \
  restartable_stage = M; case M-1:

#define TASK_YIELD_MINVT(ms) \
  restartable_stage = M; case M-1: printf("%d-%d=%d",TIME_COUNTER,elapsed_time,TIME_COUNTER-elapsed_time);\
  if (TIME_COUNTER-elapsed_time<ms) { return TASK_EXIT_RUNNING; } \
  elapsed_time = TIME_COUNTER; \
  restartable_stage = M; case M-1:

#define TASK_REPEAT_WHILE(condition) \
  if (!(condition)) { restartable_stage = M; } return TASK_EXIT_RUNNING; \
  elapsed_time = TIME_COUNTER; \
  case M-1:

#define TASK_END \
  } restartable_stage = 0; return TASK_EXIT_DONE;
