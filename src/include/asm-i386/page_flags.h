#ifndef _I386_PAGE_FLAGS_H
#define _I386_PAGE_FLAGS_H

// 对应的内容是否存在，0代表不存在，1代表存在
#define PG_Present  1

// 20位物理地址
#define PG_AddressSize 20

// 是否访问过，访问过设置为1，否则为0
// 在DIRECTORY和PAGE TABLES里面都存在
// 通过线性地址查找过
#define PG_Accessed     2
    
// 写入的符号
// Write operation.
// Only in page tables.
#define PG_Dirty        3

// Read/Write flags.
// 是否可以写入
#define PG_Write        4

// User/Supervisor flags
// 0代表Supervisor
// 1代表User和Supervisor用户都能访问
#define PG_User         5

// 控制缓存的方式
#define PG_PCD          6

// 只能作用于Page directory
// 如果被设置，那么Page directory指向了4MB大小的page frame.
#define PG_Size         7

// 防止被频繁使用的Page被交换到disk.
// cr4 PGE开启才有效
#define PG_Global       8

// PCD
// Page cache disabled.
#define PG_PCD          9

// PWT
// page write through.
#define PG_PWT          10

/*
Extended pageing.
节省page table
page directory entry指向了一个4MB的内存块
page directory address filed 10th most significant bits are used.
*/

#endif // _I386_PAGE_FLAGS_H
