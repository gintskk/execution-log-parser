#include <stdio.h>
#include <stdatomic.h>
#include <threads.h>

atomic_int flag;
atomic_int message;
int received;

int sender(void *arg) {
    atomic_store_explicit(&message, 42, memory_order_relaxed); // (A)
    atomic_store_explicit(&flag, 1, memory_order_relaxed);     // (B)
    return 0;
}

int receiver(void *arg) {
    while (atomic_load_explicit(&flag, memory_order_relaxed) != 1); // (C)
    received = atomic_load_explicit(&message, memory_order_relaxed); // (D)
    return 0;
}

int main() {
    thrd_t t1, t2;
    atomic_init(&flag, 0);
    atomic_init(&message, 0);
    received = -1;

    thrd_create(&t1, sender, NULL);
    thrd_create(&t2, receiver, NULL);

    thrd_join(t1);
    thrd_join(t2);

    printf("Receiver read message: %d\n", received);
    if (received == 42) {
        printf("Message received correctly.\n");
    } else {
        printf("Message not received correctly.\n");
    }
    return 0;
}

/*
Expected under SC:
If flag == 1, then message == 42.

But in this program (under relaxed memory order), it’s possible that:
receiver sees flag == 1 (observes (B)), but sees message == 0 (did not observe (A)).
*/