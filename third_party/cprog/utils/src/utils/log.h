#ifndef chapter_src_utils_log_h
#define chapter_src_utils_log_h

#define FAIL() \
    do { \
        *((int*)0) = 0xbaadbeef; \
    } while(0)
    
#define DCHECK(condition)   \
    do { \
        if (!(condition)) { \
            FAIL(); \
        } \
    } while(0)

#endif
