/* Necessary declaration for PA3 */

#ifndef _LOCK_H
#define _LOCK_H

#define NLOCKS 50
#define READ 0
#define WRITE 1
#define DELETE 2

#define LFREE 10
#define LUSED 11

#define USED 21
#define FREE 22

#define PRESENT 99

#define TOTAL_MAPS 250

#define YES 100
#define NO 101

extern int nextlock;

int lcreate();
int lock(int,int,int);

#define isbadlock(l)  (l<0 || l>=TOTAL_MAPS || lock_map[l] == -1)

typedef struct order_list
{
   int pid;
   int pri;
   unsigned long time;
   int lock_type;
   struct order_list *next;
}order_list_t;

typedef struct lk_tab
{
  int lstate;
  int curr_status;
  int lock_type;
  int curr_proc;
  int curr_pri;
  int read_count;
  order_list_t *head;
}lock_tab_t;

extern lock_tab_t lock_tab[];
extern  int     map_pos;
extern  int     lock_map[TOTAL_MAPS];

int priority_insert(int,int,int,int);
int ispresent(int);
int is_empty(int lock_index);
int getfirst_ll(int);
int is_write_pres(int);
int is_present(int);
void linit();
order_list_t* get_first_write(int lock_index);
order_list_t* remove_first(int lock_index);
order_list_t* get_first(int lock_index);
void print_list(int lock_index);
void putin_proc_list(int ldes1,int pid);
int is_proc_hold(int lk_index,int pid);
void lock_init(int index);

#endif
