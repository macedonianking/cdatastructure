#ifndef THIRD_PARTY_CHAPTER_SRC_CHAPTER2_INT_LIMITS_H
#define THIRD_PARTY_CHAPTER_SRC_CHAPTER2_INT_LIMITS_H

#define BITS(type)  (sizeof(type) << 3)

#define SIGNED_INT_TYPE_MIN(type)   ((type)(1 << (BITS(type) -1)))
#define SIGNED_INT_TYPE_MAX(type)   ((type)(~(SIGNED_INT_TYPE_MIN(type))))

#define UNSIGNED_INT_TYPE_MIN(type)         ((type)0)
#define UNSIGNED_INT_TYPE_MAX(type)         (~(SIGNED_INT_TYPE_MIN(type)))

#define INT_MIN_V                   SIGNED_INT_TYPE_MIN(int)
#define INT_MAX_V                   SIGNED_INT_TYPE_MAX(int)
#define UINT_MIN_V                  UNSIGNED_INT_TYPE_MIN(unsigned int)
#define UINT_MAX_V                  UNSIGNED_INT_TYPE_MAX(unsigned int)

#define SHORT_MIN_V             SIGNED_INT_TYPE_MIN(short)
#define SHORT_MAX_V             SIGNED_INT_TYPE_MAX(short)
#define USHORT_MIN_V            UNSIGNED_INT_TYPE_MIN(short)
#define USHORT_MAX_V            UNSIGNED_INT_TYPE_MIN(short)

#define CHAR_MIN_V              SIGNED_INT_TYPE_MIN(char)
#define CHAR_MAX_V              SIGNED_INT_TYPE_MAX(char)
#define UCHAR_MIN_V             UNSIGNED_INT_TYPE_MIN(unsigned char)
#define UCHAR_MAX_V             UNSIGNED_INT_TYPE_MAX(unsigned char)

#endif // THIRD_PARTY_CHAPTER_SRC_CHAPTER2_INT_LIMITS_H
