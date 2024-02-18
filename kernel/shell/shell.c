/*

    shell.c

    RainyOS Shell Program

    RainyOSTeam & RainySoftTeam RainyOS 2022~2024

*/

#include "shell.h"
#include "console.h"
#include "debug.h"
#include "string.h"
#include "fifo.h"
#include "keyboard.h"
#include "cmos.h"

#define MAX_COMMAND_LEN 100
#define MAX_ARG_NR 30

static int read_key_blocking(char *buf){
    while (fifo_status(&decoded_key) == 0);
    *buf++ = fifo_get(&decoded_key);
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
        while (*next == token) next++; // 多个token就只保留第一个，windows cmd就是这么处理的
        if (*next == 0) break; // 如果跳过完token之后结束了，那就直接退出
        argv[argc] = next; // 将首指针赋值过去，从这里开始就是当前参数
        while (*next && *next != token) next++; // 跳到下一个token
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

void shell_clear(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    console_clear();
}

void shell_ver(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    console_write("RainyOS Alpha Version 0.32 Build 15\n");
    console_write("RainyShell Alpha Version 0.12\n\n");
}

void shell_build(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    console_write("Build 15\n\n");
}

void shell_bugs(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    #ifdef HAVE_BUGS
    console_write("______________________________________\n");
    console_write("|Bugs in this version you should know|\n");
    console_write("|----------------------------------------------------------\n");
    console_write("|Note: You should know these bugs to be happy in using    |\n");
    console_write("|---------------------------------------------------------|\n");
    console_write("|1.If you type some space keys in the command line, and y-|\n");
    console_write("|ou press enter, your RainyOS will be crash.              |\n");
    console_write("|---------------------------------------------------------|\n\n");
    #else
    console_write("We haven't found bugs in RainyOS \\('v')/ \n");
    #endif
}

void shell_help(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    console_write_color("              RainyOS Help List Table            \n", rc_black, rc_magenta);
    console_write("_______________________________________________________\n");
    console_write("|NAME       |USAGE                                    |\n");
    console_write("|-----------|-----------------------------------------|\n");
    console_write("|HELP       |No argument. Show this message           |\n");
    console_write("|BUILD      |No argument. Show OS build type          |\n");
    console_write("|VER        |No argument. Show OS version             |\n");
    console_write("|CLEAR      |No argument. Clean the screen            |\n");
    console_write("|TEST       |No argument. Test system basic functions |\n");
    console_write("|CREDITS    |No argument. Show credits list           |\n");
    console_write("|BUGS       |No argument. Show bugs in this version   |\n");
    console_write("|DATE       |No argument. Show date time              |\n");
    console_write("|-----------------------------------------------------|\n\n");
}

void shell_test(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    console_write_color("Start test\n\n", rc_black, rc_green);

    console_write("QWERTYUIOPASDFGHJKLZXCVBNM\n");
    console_write("qwertyuiopasdfghjklzxcvbnm\n");
    console_write("1234567890+-*/[]{}()<>,.:;\n");
    console_write("?!@#$%^&`~|\n\n");

    console_write_color("Over\n\n", rc_black, rc_green);
}

void shell_credits(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    console_write("RainyOSTeam & RainySoftTeam 2022~2024\n");
    console_write("EVERYONE\n");
    console_write("And u\n\n");
}

void shell_date(int argc){
    if (argc != 1){
        console_write_color("Command unsupport more than 1 argument\n", rc_black, rc_red);
    }
    get_date_time();
    console_write("\n");
}

/**
 * @brief shell内建命令
 * 
 */
typedef struct builtin_cmd
{
    char *name;
    void (*func)(int argc);
} builtin_cmd_t;

builtin_cmd_t builtin_cmds[] = {
    {"clear", shell_clear},
    {"ver", shell_ver},
    {"build", shell_build},
    {"help", shell_help},
    {"test", shell_test},
    {"credits", shell_credits},
    {"bugs", shell_bugs},
    {"date", shell_date},
};

// 内建命令数量
const static int builtin_cmd_num = sizeof(builtin_cmds) / sizeof(builtin_cmd_t);

int find_cmd(char *cmd)
{
    for (int i = 0; i < builtin_cmd_num; ++i)
    {
        if (strcmp(cmd, builtin_cmds[i].name) == 0){
            return i;
        }
    }
    return -1;
}

void shell()
{
    uint8_t cmd[MAX_COMMAND_LEN];
    uint8_t *argv[MAX_ARG_NR];
    int argc = -1;

    while (true) {
        console_write("RainyOS # "); // 提示符，可自行修改

        memset(cmd, 0, MAX_COMMAND_LEN); // 清空上轮输入
        readline(cmd, MAX_COMMAND_LEN); // 读入一行

        // com就是完整的命令
        if (cmd[0] == 0) continue; // 只有一个回车，continue
        argc = cmd_parse(cmd, argv, ' ');
        // argc, argv 都拿到了
        if (argc == -1) {
            print_erro("shell: number of arguments exceed MAX_ARG_NR(30)");
            continue;
        } else if (argc == 0) {
            print_warn("space is not a command");
            continue;
        }

        int cmd_index = find_cmd(argv[0]);
        if (cmd_index < 0) {
            // 找不到该命令
            console_write("Command not found.\n\n");
        } else {
            builtin_cmds[cmd_index].func(argc);
        }
    }
    // ASSERT(0 && "unknown error on reading keys.");
}

