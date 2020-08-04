#include "server.h"

FILE *fp = NULL;

int send_head(int confd, char *filename)
{
	struct stat st;
	int ret = stat(filename, &st);
	if(-1 == ret)
	{
		perror("stat");
		return -1;
	}

	char *head[5] = {NULL};
	char lengthbuf[256] = {0};

	head[0] = "HTTP/1.1 200 OK\r\n";
	head[1] = "Server: mywebser\r\n";
	head[2] = "Content-Type: text/html; charset=utf-8\r\n";
	sprintf(lengthbuf, "Content-Length: %lu\r\n", st.st_size);
	head[3] = lengthbuf;
	head[4] = "Connection: close\r\n\r\n";
	int i = 0;
	for(i = 0; i < 5; ++i)
	{
		send(confd, head[i], strlen(head[i]), 0);
	}
	return 0;
}

int send_file(int confd, char *filename)
{
	int ret = send_head(confd, filename);
	if(-1 == ret)
	{
		return -1;
	}

	int fd = open(filename, O_RDONLY);
	if(-1 == fd)
	{
		perror("send_file open");
		return -1;
	}

	char sendbuf[1024] = {0};
	while(1)
	{
		bzero(sendbuf, sizeof(sendbuf));
		int ret = read(fd, sendbuf, sizeof(sendbuf));
		if(0 == ret)
		{
			break;
		}
		send(confd, sendbuf, ret, 0);
	}
}

int add_fd(int epfd, int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(-1 == ret)
	{
		perror("add_fd");
		return -1;
	}
	return ret;
}

int del_fd(int epfd, int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
	if(-1 == ret)
	{
		perror("del_fd");
		return -1;
	}
	return ret;
}

int server()
{
	int listfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == listfd)
	{
		perror("listfd socket");
		return -1;
	}

	struct sockaddr_in ser, cli;

	bzero(&ser, sizeof(ser));
	bzero(&cli, sizeof(cli));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(50001);
	//ser.sin_addr.s_addr = inet_addr("192.168.1.100");
	ser.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(listfd, (SA)&ser, sizeof(ser));
	if(-1 == ret)
	{
		perror("bind");
		return -1;
	}

	ret = listen(listfd, 3);
	if(-1 == ret)
	{
		perror("listen");
		return -1;
	}

	socklen_t len = sizeof(cli);
	
	int epfd = epoll_create(100);
	if(-1 == epfd)
	{
		perror("epoll_create");
		return -1;
	}

	struct epoll_event rev[100] = {0};

	add_fd(epfd, listfd);

	int i = 0;
	while(1)
	{
		int ep_ret = epoll_wait(epfd, rev, 100, -1);
		for(i = 0; i < ep_ret; ++i)
		{
			if(rev[i].data.fd == listfd)
			{
				int confd = accept(listfd, (SA)&cli, &len);
				if(-1 == confd)
				{
					perror("accept");
					return -1;
				}
				add_fd(epfd, confd);
			}
			if(rev[i].data.fd != listfd)
			{
				int confd = rev[i].data.fd;
				//收
				char recvbuf[1024] = {0};
				int nsize = recv(confd, recvbuf, sizeof(recvbuf), 0);
				if(nsize < 0)
				{
					del_fd(epfd, confd);
					close(confd);
				}

				printf("%s", recvbuf);
				fflush(stdout);

				//发
				char *method = NULL;
				char *url = NULL;
				char info[1024] = {0};
				strcpy(info, recvbuf);
				method = strtok(recvbuf, " ");  //GET / HTTP/1.1
				url = strtok(NULL, " ");
				if(!strcmp(url, "/") || strstr(url, "local_config"))
				{
					send_file(confd, "./source/1_local_config.html");
				}
				else if(strstr(url, "rs232_config"))
				{
					//send_file(confd, "./source/2_rs232_config.html");
					
					char *baud_rate = NULL;
					char *data_bits = NULL;
					char *parity_bit1 = NULL;
					char *stop_bits = NULL;
					char *remode_ser_ip = NULL;
					char *local_port = NULL;
					char *parity_bit2 = NULL;
					char *serial_packing_time = NULL;
					char *serial_packing_lenth = NULL;
					char *buf = NULL;
					
					if((buf = strstr(info, "baud_rate")))
					{
						baud_rate = strtok(buf, "&");
						data_bits = strtok(NULL, "&");
						parity_bit1 = strtok(NULL, "&");
						stop_bits = strtok(NULL, "&");
						parity_bit2 = strtok(NULL, "&");
						serial_packing_time = strtok(NULL, "&");
						serial_packing_lenth = strtok(NULL, "&");
						remode_ser_ip = strtok(NULL, "&");
						local_port = strtok(NULL, "&");
						//printf("local_port = %s\n", local_port);

						baud_rate += (strlen("baud_rate") + 1);
						data_bits += (strlen("data_bits") + 1);
						parity_bit1 += (strlen("parity_bit") + 1);
						stop_bits += (strlen("stop_bits") + 1);
						remode_ser_ip += (strlen("remode_ser_ip") + 1);
						local_port += (strlen("local_port") + 1);
						//printf("baud_rate = %s\n data_bits = %s\n parity_bit = %s\n stop_bits = %s\n remode_ser_ip = %s\n local_port = %s\n", baud_rate, data_bits, parity_bit1, stop_bits, remode_ser_ip, local_port);
					}
					sqlite3 *pdb = NULL;
					int ret = 0;
					char *perrmsg = NULL;
					char sql_cmd[1024] = {0};

					ret = sqlite3_open("/smart_geteway.db", &pdb);
					if(SQLITE_OK != ret)
					{
						fprintf(stderr, "sqlite3_exec:%s %d\n", perrmsg, __LINE__);
						sqlite3_free(perrmsg);
						sqlite3_close(pdb);
					}
						
					sprintf(sql_cmd, "insert into uartinfo values(NULL, '%s', '%s', '%s', '%s', '%s', '%s');", baud_rate, data_bits, stop_bits, parity_bit1, remode_ser_ip, local_port);
					ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, &perrmsg);
					if(-1 == ret)
					{
						perror("sqlite3_exec 280");
						return -1;
					}
					send_file(confd, "./source/2_rs232_config.html");
				}
				else if(strstr(url, "rs485_config"))
				{
					send_file(confd, "./source/3_rs485_config.html");
				}
				close(confd);
			}
		}
	}
	close(listfd);
	return 0;
}
