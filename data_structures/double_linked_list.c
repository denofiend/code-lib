/*************************************************************************
  > File Name: double_linked_list.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月17日 星期日 17时25分32秒
 ************************************************************************/

#include <stdio.h>
#include "double_linked_list.h"


int (* remove_fn) (double_linked_list_node_t const * v, int val)
{
	return v->data == value;
}

/* add one node that data is val into list*/
double_linked_list_node_t* dlist_add(double_linked_list_node_t* head, int val)
{
	double_linked_list_node_t * now = (double_linked_list_node_t*) malloc(sizeof(double_linked_list_t));

	now->data = val;
	now->prev = now->next = NULL;

	if (NULL == head)
	{
		head = now;
	}
	else
	{
		now->next = head->next;
		head->next = now;

		now->prev = head;
	}

	return head;
}

/* remove all node in list, who data is val */
void dlist_remove(double_linked_list_node_t** head, remove_fn rm, int val)
{
	
}

/* find the val the list, then change data to expect_val.*/
double_linked_list_node_t* dlist_modify(double_linked_list_node_t* head, int val, int expect_val)
{
	return head;
}

/* traversal the list, print into stdout */
void dlist_traversal(double_linked_list_node_t* head)
{
	printf("double_list_traversal(%p)\n", head);

	double_linked_list_node_t* prev = head, *next = head;

	printf(">>> next\n");
	while (*next)
	{
		printf("[%p %d]\n", next, next->data);

		next = next->next;
		if (next) prev = next;
	}

	printf("---------------\n");
	printf("<<< prev\n");

	while (*prev)
	{
		printf("[%p %d]\n", prev, prev->data);
		prev = prev->prev;
	}
	
	printf("================\n");
}

int main()
{
	double_linked_list_t * head = NULL;

	head = dlist_add(head, 1);
	dlist_traversal(head);

	return 0;
}

