#include "linklist.h"

linkList *create_linklist()
{
	linkList *plist = malloc(sizeof(linkList));
	if(NULL == plist)
	{
		perror("malloc plist fail");
		return NULL;
	}
	plist->pHead = NULL;
	plist->len = 0;
	return plist;
}

int head_insert(linkList *plist, int confd)
{
	linkNode *pTmpNode = malloc(sizeof(linkNode));
	if(NULL == pTmpNode)
	{
		perror("malloc pTmpNode fail");
		return -1;
	}

	pTmpNode->confd = confd;
	pTmpNode->count = 0;
	pTmpNode->pNext = plist->pHead;
	plist->pHead = pTmpNode;
	plist->len++;
	return 0;
}

int isEmpty(linkList *plist)
{
	return plist->len == 0;
}

int delete_point_node(linkList *plist, int confd)
{
	if(isEmpty(plist))
	{
		return 0;
	}
	linkNode *pFast = plist->pHead;
	linkNode *pSlow = plist->pHead;

	while(pFast != NULL)
	{
		if(pFast->confd == confd)
		{
			if(pFast == pSlow)
			{
				plist->pHead = pFast->pNext;
				free(pFast);
				pFast = plist->pHead;
				pSlow = plist->pHead;
			}
			else
			{
				pSlow->pNext = pFast->pNext;
				free(pFast);
				pFast = pSlow->pNext;
			}
			plist->len--;
		}
		else
		{
			pSlow = pFast;
			pFast = pFast->pNext;
		}
	}
	return 0;
}

