#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sqlite3.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <fcntl.h>

typedef struct sockaddr *(SA);



extern int server();
#endif
