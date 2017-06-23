#include "chapter6.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <memory.h>

#include <utils/apue.h>
#include <utils/utils.h>
#include <utils/list.h>
#include <utils/string_util.h>

struct unix_user_entity {
    char    *ux_name;
    char    *ux_password;
    char    *ux_groups;
    char    *ux_home;
    char    *ux_shell;
    int     ux_uid;
    int     ux_gid;
};

struct unix_user_dir {
    FILE    *file;
    char    *buffer;
    size_t  buf_size;
    struct unix_user_entity entity;
};

static struct unix_user_dir *open_unix_user_dir();
static struct unix_user_entity *read_unix_user_dir(struct unix_user_dir *dir);
static void close_unix_user_dir(struct unix_user_dir *dir);
static int get_unix_user_entity_from_uid(uid_t uid, struct unix_user_entity *entity);
static void free_unix_user_entity(struct unix_user_entity *ptr);

int chapter6_main(int argc, char **argv) {
    chapter6_2(argc, argv);
    return 0;
}

static inline void dump_tm(struct tm *tm_obj) {
    printf("year=%d, mon=%d, day=%d, hour=%d, min=%d, sec=%d\n",
           tm_obj->tm_year,
           tm_obj->tm_mon,
           tm_obj->tm_mday,
           tm_obj->tm_hour,
           tm_obj->tm_min,
           tm_obj->tm_sec);
}

time_t compute_timezone_offset() {
    time_t now;
    struct tm tm_obj;

    time(&now);
    gmtime_r(&now, &tm_obj);
    return now - mktime(&tm_obj);
}

static void test_clock_gettime() {
    struct timespec time_obj;
    time_t now;

    time(&now);
    printf("now=%ld\n", now);

    clock_gettime(CLOCK_REALTIME, &time_obj);
    printf("tv_sec=%ld, tv_nsec=%ld\n", time_obj.tv_sec,
        time_obj.tv_nsec);

    clock_getres(CLOCK_REALTIME, &time_obj);
    printf("tv_sec=%ld, tv_nsec=%ld\n", time_obj.tv_sec,
        time_obj.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &time_obj);
    printf("tv_sec=%ld, tv_nsec=%ld\n", time_obj.tv_sec,
        time_obj.tv_nsec);
}

static void test_strftime() {
    char buf[BUFSIZ];
    time_t seconds;
    struct tm tm_obj;

    time(&seconds);
    localtime_r(&seconds, &tm_obj);
    strftime(buf, BUFSIZ, "%F %T %Z", &tm_obj);
    printf("%s\n", buf);
}

void chapter6_10(int argc,
    char **argv) {
    time_t seconds;
    struct tm tm_obj;

    time(&seconds);
    localtime_r(&seconds, &tm_obj);
    dump_tm(&tm_obj);

    gmtime_r(&seconds, &tm_obj);
    dump_tm(&tm_obj);

    seconds = (time_t)0;
    localtime_r(&seconds, &tm_obj);
    printf("local_time:");
    dump_tm(&tm_obj);

    gmtime_r(&seconds, &tm_obj);
    printf("gmtime:");
    dump_tm(&tm_obj);

    test_clock_gettime();
    test_strftime();
}

void free_unix_user_entity(struct unix_user_entity *ptr) {
    free(ptr->ux_name);
    free(ptr->ux_password);
    free(ptr->ux_groups);
    free(ptr->ux_home);
    free(ptr->ux_shell);
    memset(ptr, 0, sizeof(*ptr));
}

struct unix_user_dir *open_unix_user_dir() {
    FILE *file;
    struct unix_user_dir *dir;

    if ((file = fopen("/etc/passwd", "r")) == NULL) {
        return NULL;
    }
    dir = (struct unix_user_dir*) malloc(sizeof(struct unix_user_dir));
    memset(dir, 0, sizeof(struct unix_user_dir));
    dir->file = file;
    return dir;
}

struct unix_user_entity *read_unix_user_dir(struct unix_user_dir *dir) {
    ssize_t n;
    int i;
    DEFINE_LIST_HEAD(list);
    struct string_node_t *str_node;
    struct unix_user_entity *entity;

    entity = &dir->entity;
    free_unix_user_entity(entity);
    n = getline(&dir->buffer, &dir->buf_size, dir->file);
    if (n <= 0) {
        return NULL;
    }
    if (dir->buffer[n-1] == '\n') {
        dir->buffer[n-1] = '\0';
        n--;
    }
    string_split(dir->buffer, ':', &list);
    if (list_empty(&list)) {
        return NULL;
    }

    i = 0;
    LIST_FOR_EACH_ENTRY_SAFE(str_node, &list, node) {
        switch(i) {
            case 0: {
                // username
                entity->ux_name = str_node->str;
                str_node->str = NULL;
                break;
            }
            case 1: {
                // password
                entity->ux_password = str_node->str;
                str_node->str = NULL;
                break;
            }
            case 2: {
                // uid
                entity->ux_uid = atoi(str_node->str);
                break;
            }
            case 3: {
                // gid
                entity->ux_gid = atoi(str_node->str);
                break;
            }
            case 4: {
                // groups
                entity->ux_groups = str_node->str;
                str_node->str = NULL;
                break;
            }
            case 5: {
                // home dir
                entity->ux_home = str_node->str;
                str_node->str = NULL;
                break;
            }
            case 6: {
                // bash path
                entity->ux_shell = str_node->str;
                str_node->str = NULL;
                break;
            }
            default: {
                goto finish;
            }
        }
        ++i;
    }
finish:
    LIST_FOR_EACH_ENTRY_SAFE(str_node, &list, node) {
        list_del(&str_node->node);
        free_string_node(str_node);
    }

    return entity;
}

void close_unix_user_dir(struct unix_user_dir *dir) {
    free_unix_user_entity(&dir->entity);
    free(dir->buffer);
    dir->buf_size = 0;
    free(dir);
}

int get_unix_user_entity_from_uid(uid_t uid, struct unix_user_entity *entity) {
    struct unix_user_dir *dir;
    struct unix_user_entity *ptr;
    int r;

    r = -1;
    if (!(dir = open_unix_user_dir())) {
        return -1;
    }

    while ((ptr = read_unix_user_dir(dir))) {
        if (ptr->ux_uid == uid) {
            r = 0;
            memcpy(entity, ptr, sizeof(*ptr));
            memset(ptr, 0, sizeof(*ptr));
            break;
        }
    }
    close_unix_user_dir(dir);

    return r;
}

static void chapter6_2_self() {
    struct unix_user_dir *dir;
    struct unix_user_entity *dirp, entity_obj;


    if (!(dir = open_unix_user_dir())) {
        APUE_ERR_SYS();
    }

    while ((dirp = read_unix_user_dir(dir))) {
        if (!dirp->ux_name || !dirp->ux_home || !dirp->ux_groups) {
            continue;
        }
        fprintf(stdout, "name=%s, home=%s, groups=%s, shell=%s, uid=%d\n",
            dirp->ux_name,
            dirp->ux_home,
            dirp->ux_groups,
            dirp->ux_shell,
            dirp->ux_uid);
    }
    close_unix_user_dir(dir);

    memset(&entity_obj, 0, sizeof(entity_obj));
    if (!get_unix_user_entity_from_uid(1000, &entity_obj)
        && entity_obj.ux_name
        && entity_obj.ux_groups
        && entity_obj.ux_home
        && entity_obj.ux_shell) {
        fprintf(stdout, "name=%s, home=%s, groups=%s, shell=%s, uid=%d\n",
            entity_obj.ux_name,
            entity_obj.ux_home,
            entity_obj.ux_groups,
            entity_obj.ux_shell,
            entity_obj.ux_uid);
    }
    free_unix_user_entity(&entity_obj);
}

void chapter6_2(int argc, char **argv) {
    chapter6_2_self();
}
