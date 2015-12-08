extern int epoch_count;
extern int sched_type;
extern int realtime_q;
extern int rt_q[50];
extern int rt_q_id;
extern int rt_q_st;
extern int start_debug;

#define LINUXSCHED  3
#define MULTIQSCHED 4
#define NEW_QUANTUM 100

int IS_SCHED_TYPE_M_Q(id);
