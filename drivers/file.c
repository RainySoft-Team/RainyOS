#include "file.h"
//#include "mtask.h"
#include "fifo.h"
#include "console.h"
#include "keyboard.h"

/*
int sys_open(char *pathname, uint8_t flags)
{
    // 先把pathname变成fileinfo_t
    // 由于sys_open可用于创建文件，先判断是不是创建
    int fd;
    if (flags & O_CREAT) {
        // 创建文件
        create_file(pathname);
    }
    // 再打开
    fileinfo_t *file = open_file(pathname);
    // 找到对应的fd
    task_t *task = task_now();
    for (int i = 0; i < 16; i++) {
        if (memcmp(task->fhandle[i], file, sizeof(fileinfo_t)) == 0) {
            fd = i;
        }
    }
    // 返回
    return fd;
}

int sys_close(int fd)
{
    // 本来可以调用close_file，但是要绕一个弯，不如直接操纵task
    task_t *task = task_now();
    fileinfo_t *fp = task->fhandle[fd];
    fp = NULL; // 直接清空
}
*/

int sys_read(int fd, void *buf, uint32_t cnt)
{
    if (fd < 0 || fd == 1 || fd == 2) {
        return -1; // 异常fd不接受读入
    }
    if (fd == 0) {
        // 处理标准输入
        char *keys = (char *) buf;
        uint32_t keys_read = 0;
        while (keys_read < cnt) {
            while (fifo_status(&decoded_key) == 0);
            *keys = fifo_get(&decoded_key); // 有一个键就记录一个键
            keys++;
            keys_read++;
        }
        return cnt;
    }
    /* 获得对应文件
    task_t *task = task_now();
    fileinfo_t *f = task->fhandle[fd];
    // 调用read_file
    uint8_t *tmp = (uint8_t *) kmalloc(f->size + 10);
    read_file(f, tmp);
    // 从tmp中把前cnt个复制到buf
    memcpy(buf, tmp, cnt);
    // 释放临时内存
    kfree(tmp, f->size + 10);
    return cnt; // 返回
    */
}

/*
int sys_write(int fd, const void *buf, uint32_t cnt)
{
    if (fd <= 0) {
        // 异常fd不接受写出
        return -1;
    }
    if (fd == 1 || fd == 2) {
        char *buffer = (char *) buf;
        console_write(buffer);
        return cnt;
    }
    // 获得对应文件
    task_t *task = task_now();
    fileinfo_t *f = task->fhandle[fd];
    // 从buf中把前cnt个复制到tmp
    uint8_t *tmp = (uint8_t *) kmalloc(cnt + 10);
    memcpy(tmp, buf, cnt);
    // 调用save_file
    save_file(f, tmp);
    // 释放临时内存
    kfree(tmp, cnt + 10);
    return cnt; // 返回
}

int sys_unlink(const char *pathname)
{
    // 获得对应文件
    fileinfo_t *f = open_file((char *) pathname);
    // 调用delete_file删除
    delete_file(f);
    // 关闭文件
    close_file(f);
    // 返回
    return 0;
}
*/