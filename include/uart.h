#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "tty.h"
#include "sqlite3.h"
#include "queue.h"
#include <semaphore.h>


typedef struct _tag_uart_msg
{
    char recv_msg[20];
    char sqlite_table[32];
}UART_MSG_T;

typedef struct _tag_database_info
{
	sqlite3 *pdb;
	char *perrmsg;
}DATABASE_INFO_T;

typedef struct _tag_uart_info
{
    char dev_name[128];
    int fd;
    int speed;
    int databits;
    int stopbits;
    int parity;
}UART_INFO_T;


extern UART_INFO_T *create_uart_info();
extern int uart_init(UART_INFO_T *uart_info);
extern int uart_recv_msg(UART_INFO_T *uart_info, DATABASE_INFO_T *database_info);
#endif
