#ifndef __CLEENT_H__
#define __CLEENT_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>

//typedef struct sockaddr *(SA);

//extern sem_t sem_r;

extern int web_client();
#endif
