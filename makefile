#CC=gcc
CC=arm-linux-gcc
LDFLAG=-g -o
INCLUDE_PATH=./include

#OBJ=smart_geteway
OBJ=app
OBJS+=main.o
OBJS+=common/tty.o
OBJS+=common/sqlite3.o
OBJS+=common/queue.o
OBJS+=uart.o
OBJS+=server.o
OBJS+=client.o
OBJS+=common/linklist.o
#OBJS+=common/display.o
#OBJS+=common/utf.o


$(OBJ):$(OBJS)
	$(CC) $^ $(LDFLAG) $@  -lpthread -ldl -I$(INCLUDE_PATH) 

%.o:%.c
	$(CC) $^ -c -g -o $@ -I$(INCLUDE_PATH)

.PHONY:
clean:
	rm $(OBJ) $(OBJS)
