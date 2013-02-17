/*************************************************************************
  > File Name: single_linked_list.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月17日 星期日 14时48分18秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "single_linked_list.h"


/* add one node that data is val into list*/
single_linked_list_node_t* slist_add(single_linked_list_node_t* head, int val)
{
	single_linked_list_node_t * now = (single_linked_list_node_t*)malloc(sizeof(single_linked_list_node_t));

	now->data = val;

	if (head == NULL)
		head = now;
	else
	{
		now->next = head->next;
		head->next = now;
	}

	return head;
}

int remove_fn_(single_linked_list_node_t const * v, int val)
{
	return v->data == val;
}

/* remove all node in list, who data is val */

void slist_remove(single_linked_list_node_t** head, remove_fn rm, int val)
{
	single_linked_list_node_t *entry = *head;
	single_linked_list_node_t **curr = head;

	for (curr = head; *curr; )
	{
		entry = *curr;
		if (rm(entry, val))
		{
			*curr = entry->next;
			free(entry);
		}
		else
			curr = &entry->next;
	}

}

/* find the val the list, then change data to expect_val.*/
single_linked_list_node_t* slist_modify(single_linked_list_node_t* head, int val, int expect_val)
{

	single_linked_list_node_t* p = head;
	while (p)
	{
		if (p->data == val)	p->data = expect_val;
		
		p = p->next;
	}
	return head;
}

/* traversal the list, print into stdout */
void slist_traversal(single_linked_list_node_t* head)
{
	printf("traversal <%p>\n", head);
	while (head)
	{
		printf("[%p %d]\n", head, head->data);
		head = head->next;
	}
	printf("---------------\n");
}


int main(){
	
	single_linked_list_node_t * head = NULL;

	head = slist_add(head, 1);
	slist_traversal(head);

	head = slist_add(head, 2);
	slist_traversal(head);

	slist_remove(&head, remove_fn_, 1);
	slist_traversal(head);


	head = slist_modify(head, 2, 3);
	slist_traversal(head);

	return 0;
}

