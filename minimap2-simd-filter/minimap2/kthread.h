#ifndef KTHREAD_H
#define KTHREAD_H

#ifndef __cplusplus
#include <stdbool.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

void kt_for(int n_threads, void (*func)(void*,long,int), void *data, long n);
void kt_pipeline(int n_threads, void *(*func)(void*, int, bool, void*), void *shared_data, int n_steps, bool exact_flag);

#ifdef __cplusplus
}
#endif

#endif
