#include "client.h"
#include "queue.h"
#include "common.h"
#include "linklist.h"
#include "sqlite3.h"

linkList *plinklist;

int callback(void *arg, int num, char **ptext, char **ptitle)
{
	printf("%s\n", ptext[6]);
	printf("%s\n", ptext[5]);
#if 0
	struct sockaddr_in ser = *(struct sockaddr_in *)arg;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(50000);
	ser.sin_addr.s_addr = inet_addr("192.168.1.102");
#endif	
	return 0;
}
#if 0
void *recv_heart(void *arg)
{
	while(1)
	{
		linkNode *pTmpNode = plinklist->pHead;
		while(pTmpNode != NULL)
		{
			pTmpNode->count++;
			if(pTmpNode->count > 5)
			{
				close(pTmpNode->confd);
				delete_point_node(plinklist, pTmpNode->confd);
			}
			pTmpNode = pTmpNode->pNext;
		}
		sleep(3);
	}
	return NULL;
}

void *thread_recv(void *arg)
{
	int confd = *(int *)arg;
	while(1)
	{
		char recv_msg[256] = {0};
		recv(confd, recv_msg, sizeof(recv_msg), 0);
		linkNode *pTmpNode = plinklist->pHead;
		while(pTmpNode != NULL)
		{		
			if(pTmpNode->confd == confd)
			{
				pTmpNode->count = 0;
			}
			else
			{
				head_insert(plinklist, confd);
			}
			pTmpNode = pTmpNode->pNext;
		}
	}
}
#endif

int web_client()
{
	linkList *plinklist = create_linklist();

	printf("wen_client\n");
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd)
	{
		perror("socket fail");
		return -1;
	}
	
	struct sockaddr_in ser, cli;
	bzero(&ser, sizeof(ser));

	sqlite3 *pdb = NULL;
	char *perrmsg = NULL;

	int ret = sqlite3_open("/smart_geteway.db", &pdb);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_exec:%s %d\n", perrmsg, __LINE__);
		sqlite3_free(perrmsg);
		sqlite3_close(pdb);
		return -1;
	}

	char info[6][20] = {0};
	char cmdbuff[1024] = {0};
	sprintf(cmdbuff, "select * from uartinfo where id = (select max(id) from uartinfo);");
	ret = sqlite3_exec(pdb, cmdbuff, callback, &ser, &perrmsg);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr, "sqlite3_exec:%s %d\n", perrmsg, __LINE__);
		sqlite3_free(perrmsg);
		sqlite3_close(pdb);
		return -1;
	}
	//struct sockaddr_in ser = *(struct sockaddr_in *)arg;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(50000);
	ser.sin_addr.s_addr = htonl(INADDR_ANY);
	//ser.sin_addr.s_addr = inet_addr("192.168.1.100");
	
	ret = bind(sockfd, (struct sockaddr *)&ser, sizeof(ser));
	if(-1 == ret)
	{
		perror("bidn");
		return -1;
	}

	listen(sockfd, 3);

	socklen_t len = sizeof(cli);
/*
	ret = connect(sockfd, (struct sockaddr *)&ser, sizeof(ser));
	if(-1 == ret)
	{
		perror("connect");
		return -1;
	}
*/
	printf("accept\n");
	int confd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if(-1 == confd)
	{
		perror("accept");
		return -1;
	}

	printf("accept jieshu\n");
	dataType recv_msg[20] = {0};
#if 0
	pthread_t tid, tid1;
	pthread_create(&tid, NULL, recv_heart, NULL);
	pthread_create(&tid1, NULL, thread_recv, &confd);
#endif
	printf("-------------\n");
	while(1)
	{
		bzero(recv_msg, sizeof(recv_msg));
		sem_wait(&sem_r);
		printf("leave_queue:%d\n", plist->len);
		leave_queue(plist, recv_msg);
		printf("client:%p\n", plist);
		printf("leave_queue1:%d\n", plist->len);
		printf("recv_msg:%s\n", recv_msg);
		send(confd, recv_msg, strlen(recv_msg) + 1, 0);
	}
	close(confd);
	close(sockfd);
}
