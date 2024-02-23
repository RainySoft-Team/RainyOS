#ifndef _UNISTD_H_
#define _UNISTD_H_

#ifdef ___NoTsTd___

void _exit(int retval);
int exec(char *progname);
void clear();
void ls();
void lspci();
uint8_t *datetime();
void lscpu();
void shutdown();
void reboot();

#endif

#endif