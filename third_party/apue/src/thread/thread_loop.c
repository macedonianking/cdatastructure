#include "thread/thread_loop.h"

#include <stdlib.h>

#include "utils/log.h"
#include "utils/list.h"

#define MAX_THREAD_COUNT            (1 << 16)
#define BITMAP_ITEM_BITS            (sizeof(unsigned int) << 3)
#define THREAD_ID_BITMAP_SIZE       (MAX_THREAD_COUNT / BITMAP_ITEM_BITS)

#define HASH_SIZE   29

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
        hash += (*ptr++) * 31;
    }

    return hash % HASH_SIZE;
}

static thread_id_node *get_thread_id_node_locked(ThreadId tid) {
    list_head *head;
    thread_id_node *ptr;

    head = &thread_id_map[hash_id(tid)];
    LIST_FOR_EACH_ENTRY(ptr, head, t_node) {
        if (is_thread_id_equal(tid, ptr->t_tid)) {
            return ptr;
        }
    }
    return NULL;
}

static int get_next_available_thread_id() {
    int start, end;
    int index;
    int curr;
    int ret;

    start = g_next_thread_id_seq / BITMAP_ITEM_BITS;
    index = g_next_thread_id_seq % BITMAP_ITEM_BITS;
    end = THREAD_ID_BITMAP_SIZE;
    for (curr = start; curr < end; ++curr) {
        if (g_thread_id_bitmap[curr] + 1 == 0) {
            continue;
        }

        for (; index < BITMAP_ITEM_BITS; ++index) {
            if ((g_thread_id_bitmap[curr] & (1u << index)) == 0) {
                goto find;
            }
        }
        index = 0;
    }

    index = 0;
    for (curr = 0; curr < start; ++curr) {
        if (g_thread_id_bitmap[curr] + 1 == 0) {
            continue;
        }

        for (; index < BITMAP_ITEM_BITS; ++index) {
            if ((g_thread_id_bitmap[curr] & (1u << index)) == 0) {
                goto find;
            }
        }
        index = 0;
    }

    // too many threads.
    DCHECK_UNREACHED();

find:
    ret = curr * BITMAP_ITEM_BITS + index;
    g_next_thread_id_seq = ret + 1;
    if (g_next_thread_id_seq >= MAX_THREAD_COUNT) {
        g_next_thread_id_seq = 0;
    }
    return ret;

    // shoult not be here.
    DCHECK_UNREACHED();
    return -1;
}

static void clear_thread_id_bitmap(int id) {
    int curr;
    unsigned int flag;

    curr = id / BITMAP_ITEM_BITS;
    flag = (1u << (id % BITMAP_ITEM_BITS));
    g_thread_id_bitmap[curr] &= flag;
}

int thread_loop_enqueue_therad_id(ThreadId tid) {
    list_head *head;
    thread_id_node *ptr;
    int r;

    r = -1;
    ThreadLoopGlobalLockAcquire();
    ptr = get_thread_id_node_locked(tid);
    if (ptr == NULL) {
        head = &thread_id_map[hash_id(tid)];
        r = 0;
        ptr = (thread_id_node*) malloc(sizeof(thread_id_node));
        INIT_LIST_HEAD(&ptr->t_node);
        ptr->t_tid = tid;
        ptr->t_cid = get_next_available_thread_id();
        list_add(&ptr->t_node, head);
    }
    ThreadLoopGlobalLockRelease();
    return r;
}

int thread_loop_dequeue_therad_id(ThreadId tid) {
    thread_id_node *ptr;
    int r;

    r = -1;
    ThreadLoopGlobalLockAcquire();
    ptr = get_thread_id_node_locked(tid);
    if (ptr != NULL) {
        r = 0;
        clear_thread_id_bitmap(ptr->t_cid);
        list_del(&ptr->t_node);
        free(ptr);
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

int custom_thread_id() {
    int cid = -1;
    thread_id_node *ptr;

    ThreadLoopGlobalLockAcquire();
    ptr = get_thread_id_node_locked(current_thread_id());
    if (ptr) {
        cid = ptr->t_cid;
    }
    ThreadLoopGlobalLockRelease();

    return cid;
}

void thread_params_default_params_dctor(void *args) {
    free(args);
}
