#ifndef __TTY_H__
#define __TTY_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
/*
 * linux
 */

extern int opendev(char *dev);
extern void set_speed(int fd, int speed);
extern int set_parity(int fd,int databits,int stopbits,int parity);
extern void set_port(int fd, int speed, int databits, int stopbits, int parity);
#endif
