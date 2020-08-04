#include "queue.h"

QUEUE_LIST *create_queue()
{
	QUEUE_LIST *plist = malloc(sizeof(QUEUE_LIST));
	if(NULL == plist)
	{
		perror("malloc plist fail");
		return NULL;
	}

	plist->len = 0;
	plist->pFront = NULL;
	plist->pRear = NULL;

	return plist;
}

int is_empty(QUEUE_LIST *plist)
{
	return plist->len == 0;
}

int enqueue(QUEUE_LIST *plist, dataType *data)
{
	QUEUE_NODE *pTmpNode = malloc(sizeof(QUEUE_NODE));
	if(NULL == pTmpNode)
	{
		perror("malloc pTmpNode fail");
		return -1;
	}

	strcpy(pTmpNode->data, data);
	pTmpNode->pNext = NULL;

	if(is_empty(plist))
	{
		plist->pFront = pTmpNode;
		plist->pRear = pTmpNode;
	}
	else
	{
		plist->pRear->pNext = pTmpNode;
		plist->pRear = pTmpNode;
	}
	plist->len++;
	return 0;
}

int leave_queue(QUEUE_LIST *plist, dataType *data)
{
	QUEUE_NODE *pTmpNode = plist->pFront;
	if(is_empty(plist))
	{
		return -1;
	}

	plist->pFront = pTmpNode->pNext;
	if(data != NULL)
	{
		strcpy(data, pTmpNode->data);
	}
	free(pTmpNode);

	if(plist->pFront == NULL)
	{
		plist->pRear = NULL;
	}

	plist->len--;
	return 0;
}
