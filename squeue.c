#include "squeue.h"
#include <stdlib.h>

int rand_size(int size)
{
    int end = RAND_MAX / size;
    end *= size;
    int r;
    while ((r = rand()) >= end)
        ;
    return r % size;
}

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    int len = q_size(head);
    LIST_HEAD(shuffled);
    struct list_head *old;
    do {
        old = head->next;
        int rnd = rand_size(len);
        for (int i = rnd; i > 0; i--)
            old = old->next;
        list_move(head->prev, old->prev);
        list_move(old, &shuffled);
        len--;
    } while (len != 0);
    list_splice_tail(&shuffled, head);
}