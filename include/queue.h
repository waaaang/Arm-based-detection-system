#ifndef __SEQ_QUEUE_H__
#define __SEQ_QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char dataType;

typedef struct _tag_node
{
	dataType data[256];
	struct _tag_node *pNext;
}QUEUE_NODE;

typedef struct _tag_list_
{
	QUEUE_NODE *pFront;
	QUEUE_NODE *pRear;
	int len;
}QUEUE_LIST;


extern QUEUE_LIST *create_queue();
extern int is_empty(QUEUE_LIST *plist);
extern int enqueue(QUEUE_LIST *plist, dataType *data);
extern int leave_queue(QUEUE_LIST *plist, dataType *data);

#endif
