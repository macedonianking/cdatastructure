#ifndef CHAPTER14_RECORD_LOCK_H
#define CHAPTER14_RECORD_LOCK_H

void chapter14_record_lock(int argc, char **argv);

void chapter14_record_lock_1(int argc, char **argv);

int record_lock_wr_lock(int fd);
int record_lock_rd_lock(int fd);
int record_lock_is_rd_locked(int fd);
int record_lock_is_wr_locked(int fd);

#endif // CHAPTER14_RECORD_LOCK_H
