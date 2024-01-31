#include "common.h"
#include "shell.h"
#include "console.h"
//#include "fat16.h"
#include "heap.h"
#include "string.h"
#include "fifo.h"
#include "keyboard.h"

#define MAX_COMMAND_LEN 100
#define MAX_ARG_NR 30

#define true 1
#define false 0

static int read_key_blocking(char *buf){
    while (fifo_status(&decoded_key) == 0);
    *buf++ == fifo_get(&decoded_key);
    return 0;
}

static int cmd_parse(uint8_t *cmd_str, uint8_t **argv, uint8_t token) // 用uint8_t是因为" "使用8位整数
{
    int arg_idx = 0;
    while (arg_idx < MAX_ARG_NR) {
        argv[arg_idx] = NULL;
        arg_idx++;
    } // end of while
    uint8_t *next = cmd_str; // 下一个字符
    int argc = 0; // 这就是要返回的argc了
    while (*next) { // 循环到结束为止
        while (*next == token) *next++; // 多个token就只保留第一个，windows cmd就是这么处理的
        if (*next == 0) break; // 如果跳过完token之后结束了，那就直接退出
        argv[argc] = next; // 将首指针赋值过去，从这里开始就是当前参数
        while (*next && *next != token) *next++; // 跳到下一个token
        if (*next) { // 如果这里有token字符
            *next++ = 0; // 将当前token字符设为0（结束符），next后移一个
        }
        if (argc > MAX_ARG_NR) return -1; // 参数太多，超过上限了
        argc++; // argc增一，如果最后一个字符是空格时不提前退出，argc会错误地被多加1
    }
    return argc;
}

static void readline(char *buf, int cnt)
{
    char *pos = buf;
    while (read_key_blocking(pos) != -1 && (pos - buf) < cnt) { // 没打够字符并且不出错
        switch (*pos) {
            case '\n':
            case '\r':
                *pos = 0; // 读完了，好耶！
                console_putc_color('\n', rc_black, rc_white); // 只打印一个\n
                return;
            
            case '\b':
                if (buf[0] != '\b') { // 哥们怎么全删完了
                    --pos; // 退回上一个字符
                    console_putc_color('\b', rc_black, rc_white); // 打印一个\b
                }
                break;
            
            default:
                // 其他
                console_putc_color(*pos, rc_black, rc_white);
                pos++;
        }
    }
}

/*
static void cmd_ls(int argc, uint8_t **argv)
{
    fileinfo_t *file_infos = (fileinfo_t *) kmalloc(32 * 48);
    int filenum = read_root_dir_files(file_infos);
    for (int i = 0; i < filenum; i++) {
        console_write(file_infos[i].name);
        console_write("\n");
    }
}

static void cmd_touch(int argc, uint8_t **argv)
{
    if (argc != 2) {
        console_write("touch: only support 1 argument\n");
        return;
    }
    int fd = sys_open(argv[1], O_CREAT);
    sys_close(fd);
}
*/

void shell()
{
    uint8_t com[MAX_COMMAND_LEN];
    uint8_t *argv[MAX_ARG_NR];
    int argc = -1;
    
    console_clear();

    while (true) {
        console_write("RainyOS> "); // 提示符，可自行修改
        
        memset(com, 0, MAX_COMMAND_LEN); // 清空上轮输入
        readline(com, MAX_COMMAND_LEN); // 读入一行

        // com就是完整的命令
        if (com[0] == 0) continue; // 只有一个回车，continue
        argc = cmd_parse(com, argv, ' ');
        // argc, argv 都拿到了
        if (argc == -1) {
            console_write("shell: error: number of arguments exceed MAX_ARG_NR(30)\n");
            continue;
        }
        /*
        if (!strcmp("dir", argv[0]) || !strcmp("ls", argv[0])) {
            cmd_ls(argc, argv);
        } else if (!strcmp("touch", argv[0])) {
            cmd_touch(argc, argv);
        }
        */
        if (!strcmp("clear", argv[0])){
            console_clear();
        }
        else if (!strcmp("ver", argv[0])) {
            console_write("Build 15");
        }
        else console_write("Command not found.\n");
    }
    ASSERT(0 && "unknown error on reading keys.");
}

