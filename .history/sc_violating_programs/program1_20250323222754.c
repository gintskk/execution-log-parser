#include <stdio.h>
#include <threads.h>
#include <stdatomic.h>

atomic_int x, y;
atomic_int r1, r2;

static int thread1(void *arg) {
    atomic_store_explicit(&x, 1, memory_order_relaxed); // (A)
    atomic_store_explicit(&y, 2, memory_order_relaxed); // (C)
    return 0;
}

static int thread2(void *arg) {
    r2 = atomic_load_explicit(&y, memory_order_relaxed); // (B)
    r1 = atomic_load_explicit(&x, memory_order_relaxed); // (D)
    return 0;
}

int main() {
    thrd_t t1, t2;
    atomic_init(&x, 0);
    atomic_init(&y, 0);
    r1 = 0;
    r2 = 0;

    for (int i = 0; i < 10; i++) {
        atomic_store_explicit(&x, 0, memory_order_seq_cst);
        atomic_store_explicit(&y, 0, memory_order_seq_cst);
        r1 = 0;
        r2 = 0;

        thrd_create(&t1, thread1, NULL);
        thrd_create(&t2, thread2, NULL);

        thrd_join(t1);
        thrd_join(t2);
    }
    printf("r1 = %d, r2 = %d\n", r1, r2);
    printf("Main thread is finishing\n");
    return 0;
}


/*
In this program SC violation occur when r2 = 2 and r1 = 0. 
The order of writes to the addereses of x any in memory can be swapped due to a memeory_order_relaxed flag. 
This program needs to be run inside c11
*/