#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *ele, *safe;
    list_for_each_entry_safe (ele, safe, l, list)
        q_release_element(ele);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }
    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_first_entry(head, element_t, list);
    list_del(&ele->list);
    if (sp) {
        size_t len = strlen(ele->value);
        len = len < bufsize - 1 ? len : bufsize - 1;
        strncpy(sp, ele->value, len);
        *(sp + len) = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_last_entry(head, element_t, list);
    list_del(&ele->list);
    if (sp) {
        size_t len = strlen(ele->value);
        len = len < bufsize - 1 ? len : bufsize - 1;
        strncpy(sp, ele->value, len);
        *(sp + len) = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head == NULL)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head) {
        len++;
    }
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast;
    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next) {
        slow = slow->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *li, *safe;
    list_for_each_safe (li, safe, head) {
        element_t *ele = list_entry(li, element_t, list);
        struct list_head *ptr = li->next;
        bool flag = false;
        while (ptr != head &&
               strcmp(ele->value, list_entry(ptr, element_t, list)->value) ==
                   0) {
            list_del(ptr);
            q_release_element(list_entry(ptr, element_t, list));
            ptr = li->next;
            flag = true;
        }
        safe = li->next;
        if (flag) {
            list_del(li);
            q_release_element(list_entry(li, element_t, list));
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *li, *safe;
    int i = 0;
    list_for_each_safe (li, safe, head) {
        if (i & 1) {
            list_move(li, li->prev->prev);
        }
        i++;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *li, *safe, *last = head->prev;
    list_for_each_safe (li, safe, head) {
        if (li == last)
            return;
        list_move(li, last);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;
    LIST_HEAD(khead);
    khead.next = head;
    struct list_head *li, *safe;
    int i = 0;
    list_for_each_safe (li, safe, head) {
        int tmp = i % k;
        if (tmp == 0) {
            khead.next->next = li;
            khead.next = li;
        } else if (tmp == k - 1) {
            khead.prev = li;
            q_reverse(&khead);
        }
        i++;
    }
}

void l_merge_two(struct list_head *first,
                 struct list_head *second,
                 struct list_head *head)
{
    while (!list_empty(first) && !list_empty(second)) {
        element_t *ele1 = list_entry(first->next, element_t, list);
        element_t *ele2 = list_entry(second->next, element_t, list);
        if (strcmp(ele1->value, ele2->value) <= 0)
            list_move_tail(first->next, head);
        else
            list_move_tail(second->next, head);
    }
    if (!list_empty(first))
        list_splice_tail_init(first, head);
    else
        list_splice_tail_init(second, head);
}


/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head, *fast = head;
    do {
        slow = slow->next;
        fast = fast->next->next;
    } while (fast != head && fast->next != head);

    LIST_HEAD(first);
    LIST_HEAD(second);
    list_splice_tail_init(head, &second);
    list_cut_position(&first, &second, slow);
    q_sort(&first);
    q_sort(&second);
    l_merge_two(&first, &second, head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;
    struct list_head *cur = head, *safe;
    element_t *max = NULL;
    int sum = 0;
    do {
        cur = cur->prev;
        safe = cur->prev;
        element_t *ele = list_entry(cur, element_t, list);
        if (!max || strcmp(ele->value, max->value) >= 0) {
            max = ele;
            sum++;
        } else {
            list_del(cur);
            q_release_element(ele);
            cur = safe->next;
        }
    } while (safe != head);

    return sum;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain),
                   *ctx = NULL;
    int sum = first->size;
    list_for_each_entry (ctx, head, chain) {
        if (ctx == first)
            continue;
        sum += ctx->size;
        list_splice_tail_init(ctx->q, first->q);
    }
    q_sort(first->q);
    return sum;
}
