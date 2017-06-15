#include "thread/thread_loop.h"

#include <stdlib.h>

#include "utils/log.h"
#include "utils/list.h"

#define MAX_THREAD_COUNT            (1 << 16)
#define THREAD_ID_BITMAP_SIZE       (MAX_THREAD_COUNT / (sizeof(unsigned int)))

#define HASH_SIZE   31

typedef struct thread_id_node {
    list_head   t_node;
    ThreadId    t_tid;
    int         t_cid;
} thread_id_node;

static unsigned int g_thread_id_bitmap[THREAD_ID_BITMAP_SIZE] = {0};
static int g_next_thread_id_seq = 0;
static list_head thread_id_map[HASH_SIZE];

static inline unsigned int hash_id(ThreadId tid) {
    unsigned int hash;
    unsigned char *ptr, *end;

    hash = 0;
    ptr = (unsigned char *)(void *)(&tid);
    end = (unsigned char *)(void *)(&tid + 1);
    while (ptr < end) {
        hash += (*ptr) * 31;
    }

    return hash % HASH_SIZE;
}

static int get_next_available_thread_id() {
    int start, end;
    int curr;
    int ret;

    start = g_next_thread_id_seq / (sizeof(unsigned int));
    end = THREAD_ID_BITMAP_SIZE;
    for (curr = start; curr < end; ++curr) {
        if (g_thread_id_bitmap[curr] + 1 == 0) {
            continue;
        }
        goto find;
    }

    for (curr = 0; curr < start; ++curr) {
        if (g_thread_id_bitmap[curr] + 1 == 0) {
            continue;
        }
        goto find;
    }
    
    // too many threads.
    DCHECK_UNREACHED();
find:
    for (int i = 0; i < sizeof(unsigned int); ++i) {
        if ((g_thread_id_bitmap[curr] & (1 << i)) != 0) {
            continue;
        }
        ret = curr * sizeof(unsigned int) + i;
        g_next_thread_id_seq = ret + 1;
        if (g_next_thread_id_seq >= MAX_THREAD_COUNT) {
            g_next_thread_id_seq = 0;
        }
        return ret;
    }

    // shoult not be here.
    DCHECK_UNREACHED();
    return -1;
}

static void clear_thread_id_bitmap(int id) {
    int curr;
    unsigned int flag;

    curr = id / sizeof(unsigned int);
    flag = (1u << (id % sizeof(unsigned int)));
    g_thread_id_bitmap[curr] &= flag;
}

int thread_loop_enqueue_therad_id(ThreadId tid) {
    unsigned int v = hash_id(tid);
    list_head *head;
    thread_id_node *ptr, *curr;

    int r = -1;

    head = &thread_id_map[v];
    curr = NULL;
    ThreadLoopGlobalLockAcquire();
    LIST_FOR_EACH_ENTRY(ptr, head, t_node) {
        if (ptr->t_tid == tid) {
            curr = ptr;
            break;
        }
    }
    if (curr == NULL) {
        r = 0;
        curr = (thread_id_node*) malloc(sizeof(thread_id_node));
        INIT_LIST_HEAD(&curr->t_node);
        curr->t_tid = tid;
        curr->t_cid = get_next_available_thread_id();
        list_add(&curr->t_node, head);
    }
    ThreadLoopGlobalLockRelease();
    return r;
}

int thread_loop_dequeue_therad_id(ThreadId tid) {
    list_head *head;
    thread_id_node *ptr;
    int r;

    r = -1;
    head = &thread_id_map[hash_id(tid)];
    ThreadLoopGlobalLockAcquire();
    LIST_FOR_EACH_ENTRY(ptr, head, t_node) {
        if (is_thread_id_equal(ptr->t_tid, tid)) {
            clear_thread_id_bitmap(ptr->t_tid);
            list_del(&ptr->t_node);
            free(ptr);
            r = 0;
            break;
        }
    }
    ThreadLoopGlobalLockRelease();

    return r;
}

void ThreadLoopInitialize() {
    ThreadLoopInitializeEnterImpl();

    for (int i = 0; i < HASH_SIZE; ++i) {
        INIT_LIST_HEAD(thread_id_map + i);
    }
    thread_loop_enqueue_therad_id(current_thread_id());

    ThreadLoopInitializeEnterImpl();
}

