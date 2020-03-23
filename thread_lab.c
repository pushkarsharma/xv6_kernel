#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "x86.h"

int max_pass_val = 6;
int global_pass_val = 0;
void *stack_arr[10];
int thread_count = 0;
int turn = 0;
int thread_total = 4;

struct x86_spinlock
{
	uint turn;
}x86_lock;

void lock_init(struct x86_spinlock *lock)
{
	xchg(&lock->turn, 0);
}

void lock_acquire(struct x86_spinlock *lock, int t_num)
{
	while(xchg(&lock->turn, lock->turn) != t_num);
}

void lock_release(struct x86_spinlock *lock)
{
	xchg(&lock->turn, (lock->turn+1)%thread_total);
}

struct thread_args {
	int thread_num;
};

void*
frisbee(void *arg)
{
	int t_num = ((struct thread_args*)arg)->thread_num;
	
	while(global_pass_val+(thread_total-1) < max_pass_val)
	{
		lock_acquire(&x86_lock, t_num);
		printf(1, "Pass number no: %d, Thread %d is passing the token to thread %d\n", global_pass_val, t_num, (t_num + 1)%thread_total);
		global_pass_val++;
		lock_release(&x86_lock);
	}
	return arg;
}

int
thread_create(void * (*start_routine)(void *), void * arg)
{
	void * stack = (void *)malloc(4096);
	int tid = clone(stack, 4096);
	if(tid == 0)
	{
		start_routine(arg);
		free(stack);
		zombify();
	}
	return 0;
}

int
main(int argc, char **argv)
{
	thread_total = atoi(argv[1]);
	max_pass_val = atoi(argv[2]);
	lock_init(&x86_lock);
	struct thread_args* args[thread_count];
	for(thread_count = 0; thread_count<thread_total; thread_count++)
	{
		struct thread_args* arg = (struct thread_args*)malloc(sizeof(struct thread_args));
		arg->thread_num = thread_count;
		args[thread_count] = arg;
		thread_create(frisbee, (void*)arg);
	}
	wait();
	printf(1, "Exiting-------------\n");
	for (thread_count = 0; thread_count < thread_total; thread_count++)
	{
		free(args[thread_count]);
	}
	exit();
}