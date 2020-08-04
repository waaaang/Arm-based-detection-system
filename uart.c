#include "uart.h"
#include "common.h"
#include "client.h"

UART_INFO_T *create_uart_info()
{
    UART_INFO_T *uart_info = malloc(sizeof(UART_INFO_T));
    if(NULL == uart_info)
    {
        perror("malloc uart_info fail");
        return NULL;
    }
    uart_info->fd = 0;
    strcpy(uart_info->dev_name, "/dev/ttySAC0");
    uart_info->speed = 115200;
    uart_info->databits = 8;
    uart_info->stopbits = 1;
    uart_info->parity = 'n';
    return uart_info;
}

int fd_set_nonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    flag = flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
    return 0;
}

int uart_init(UART_INFO_T *uart_info)
{
    //UART_INFO_T *uart_info = create_uart_info();

    uart_info->fd = opendev(uart_info->dev_name);
    set_port(uart_info->fd, uart_info->speed, uart_info->databits, uart_info->stopbits, uart_info->parity);
}



int uart_recv_msg(UART_INFO_T *uart_info, DATABASE_INFO_T *database_info)
{

	//QUEUE_LIST *plist = create_queue();

    //UART_MSG_T msg;

    //set nonblock
    fd_set_nonblock(uart_info->fd);
	int ret;
    char send_msg[10] = "nihao";
    char recv_msg[20] = {0};
	
#if 0	
	sqlite3 *pdb = NULL;
	char *perrmsg = NULL;

	ret = sqlite3_open("smart_geteway.db", &pdb);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_open:%s\n", sqlite3_errmsg(pdb));
		return ;
	}
#endif
	
	char cmdbuff[1024] = {0};
	sprintf(cmdbuff, "create table if not exists uartmsg(id integer primary key asc, msg char);");
	ret = sqlite3_exec(database_info->pdb, cmdbuff, NULL, NULL, &database_info->perrmsg);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_exec:%s\n", database_info->perrmsg);
		sqlite3_free(database_info->perrmsg);
		sqlite3_close(database_info->pdb);
		return ;
	}
	while(1)
    {
        //往串口写
        write(uart_info->fd, send_msg, strlen(send_msg));
		//bzero(send_msg, sizeof(send_msg));
		sleep(1);
        //从串口读
		if((ret = read(uart_info->fd, recv_msg, sizeof(recv_msg))) > 0)
		{
			//strcat(recv_msg, "+++");
			//write(uart_info->fd, recv_msg, strlen(recv_msg));
			//
			printf("%s\n", recv_msg);
			sprintf(cmdbuff, "insert into uartmsg values (NULL, \'%s\')", recv_msg);
			printf("%s\n", cmdbuff);
			ret = sqlite3_exec(database_info->pdb, cmdbuff, NULL, NULL, &database_info->perrmsg);
			if(SQLITE_OK != ret)
			{
				//	fprintf(stderr, "sqlite3_exec:%s\n", database_info->perrmsg);
				fprintf(stderr, "sqlite3_exec:%s %d\n", database_info->perrmsg, __LINE__);
				sqlite3_free(database_info->perrmsg);
				sqlite3_close(database_info->pdb);
				return ;
			}
			printf("**************\n");
			printf("enqueue:%d\n", plist->len);
			enqueue(plist, recv_msg);
			printf("uart:%p\n", plist);
			printf("1111:%s\n", recv_msg);
			sem_post(&sem_r);
			printf("enqueue1:%d\n", plist->len);
			printf("///////////////\n");
		}
		bzero(recv_msg, sizeof(recv_msg));
    }
	sqlite3_close(database_info->pdb);
}
