#ifndef __LINKLIST_H__
#define __LINKLIST_H__

#include <stdio.h>
#include <stdlib.h>


typedef struct node
{
	int confd;
	int count;
	struct node *pNext;
}linkNode;

typedef struct listen
{
	linkNode *pHead;
	int len;
}linkList;

extern linkList *create_linklist();
extern int head_insert(linkList *plist, int confd);
extern int isEmpty(linkList *plist);
extern int delete_point_node(linkList *plist, int confd);
#endif
