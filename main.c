#include "uart.h"
#include "server.h"
#include "client.h"
#include <semaphore.h>
#include "common.h"
#include "display.h"
#include "utf.h"

DATABASE_INFO_T database_info;
sem_t sem_r;

QUEUE_LIST *plist, *plist1;

# if 1 
void *thread_uart(void *arg)
{
	//QUEUE_LIST *plist = (QUEUE_LIST *)arg;
	
	UART_INFO_T *uart_info = create_uart_info();
	while(1)
	{
		if(!uart_init(uart_info))
		{
			sleep(5);
			continue;
		}
		uart_recv_msg(uart_info, &database_info);
	}
	//sem_post(&sem_r);
}

#endif
# if 1
void *thread_client(void *arg)
{
	//sem_wait(&sem_r);
	web_client();
}
#endif

# if 1
void *thread_server(void *arg)
{
	server();
}
#endif

#if 0
void *thread_gui(void *arg)
{
	u32 *addr = init_fb();
	while(1)
	{
	char buf[256] = {0};
	leave_queue(plist1, buf);
	bzero(&utf8_info, sizeof(UTF8_INFO));
	utf8_info.width = 32;
	utf8_info.height = 32;
	strcpy(utf8_info.path, "/utf");
	init_utf8(&utf8_info);

	display_show_utf8_str(addr, &utf8_info, 470, 200, buf, rgb888(0,0,0), rgb888(255, 255, 255));
	}
}
#endif

int main(void)
{
	//数据库初始化
	database_info.pdb = NULL;
	database_info.perrmsg = NULL;
	int ret = 0;

	ret = sqlite3_open("/smart_geteway.db", &database_info.pdb);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_open:%s\n", sqlite3_errmsg(database_info.pdb));
		return ;
	}
	
	char cmdbuff[1024] = {0};
	sprintf(cmdbuff, "create table if not exists uartmsg(id integer primary key asc, msg char);");
	ret = sqlite3_exec(database_info.pdb, cmdbuff, NULL, NULL, &database_info.perrmsg);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_exec23:%s\n", database_info.perrmsg);
		sqlite3_free(database_info.perrmsg);
		sqlite3_close(database_info.pdb);
		return ;
	}

	sprintf(cmdbuff, "create table if not exists uartinfo(id integer primary key asc, speed int, databits int, stopbits int, parity int, clientaddr char, clientport int);");
	ret = sqlite3_exec(database_info.pdb, cmdbuff, NULL, NULL, &database_info.perrmsg);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_exec:%s\n", database_info.perrmsg);
		sqlite3_free(database_info.perrmsg);
		sqlite3_close(database_info.pdb);
		return ;
	}

	//创建缓存队列	
	plist = create_queue();
	//plist1 = plist;
	//server();

//	web_client(plist);
	
	pthread_t tid1, tid2, tid3, tid4;
	sem_init(&sem_r, 0, 0);

	pthread_create(&tid1, NULL, thread_uart, NULL);
	pthread_create(&tid2, NULL, thread_client, NULL);
	pthread_create(&tid3, NULL, thread_server, NULL);
//	pthread_create(&tid4, NULL, thread_gui, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
//	pthread_join(tid4, NULL);

	sem_destroy(&sem_r);
   
	return 0;
}

