#include<sched.h>
int sched_mode;

int IS_SCHED_TYPE_M_Q(id)
{
	return (id == MULTIQSCHED);
}
