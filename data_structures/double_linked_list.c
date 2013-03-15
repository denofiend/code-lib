/*************************************************************************
  > File Name: double_linked_list.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月17日 星期日 17时25分32秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "double_linked_list.h"


int remove_fn_ (double_linked_list_node_t const * v, int val)
{
	return val == v->data;
}

/* add one node that data is val into list*/
double_linked_list_node_t* dlist_add(double_linked_list_node_t* head, int val)
{
	double_linked_list_node_t * now = (double_linked_list_node_t*) malloc(sizeof(double_linked_list_node_t));

	now->data = val;
	now->prev = now->next = NULL;

	if (NULL == head)
	{
		head = now;
	}
	else
	{
		//
		now->next = head->next;
		now->prev = head;

		if (head->next)
			head->next->prev = now; 

		head->next = now;

	}

	return head;
}

/* remove all node in list, who data is val */
void dlist_remove(double_linked_list_node_t** head, remove_fn rm, int val)
{
	double_linked_list_node_t **curr = head;
	double_linked_list_node_t *entry;
	
	while (*curr)
	{
		entry = *curr;

		if (rm(entry, val))
		{

			*curr = entry->next;
			
			if (entry->next)
				entry->next->prev = entry->prev;

			free(entry);
		}
		else
		{
			curr = &entry->next;
		}
	}
}

/* find the val the list, then change data to expect_val.*/
double_linked_list_node_t* dlist_modify(double_linked_list_node_t* head, int val, int expect_val)
{
	double_linked_list_node_t* p = head;

	while (p)
	{
		if (p->data == val)
			p->data = expect_val;
		p = p->next;

	}

	return head;
}

/* traversal the list, print into stdout */
void dlist_traversal(double_linked_list_node_t* head)
{
	printf("double_list_traversal(%p)\n", head);

	double_linked_list_node_t* prev = head, *next = head;

	printf(">>> next\n");
	while (next)
	{
		printf("[%p %d]\n", next, next->data);

		next = next->next;
		if (next) prev = next;
	}

	printf("---------------\n");
	printf("<<< prev\n");

	while (prev)
	{
		printf("[%p %d]\n", prev, prev->data);
		prev = prev->prev;
	}
	
	printf("================\n");
}

/* reversion tow adjacent node int the double linked list */
void dlist_reversion(double_linked_list_node_t** head)
{
	double_linked_list_node_t *f, *s;

	if (NULL == *head)
	{
		printf("empty list, not need reversion\n");
		return;
	}

	f = *head;
	s = f->next;

	//printf("f, s -> [%p %p]\n", f, s);
	
	// if s is not null, set *head = s
	if (s)
		*head = s;

	while (f && s)
	{
		//printf("<%p, %p> [%p, %p] [%p, %p]\n", f, s, f->prev, f->next, s->prev, s->next);

		// if s->next if not null, set s->next->prev = f
		if (s->next)
		{
			s->next->prev = f;
		}

		// if f->prev if not null, set f->prev->next = s
		if (f->prev)
		{
			f->prev->next = s;
		}

		// swap the prev of f, s
		s->prev = f->prev;
		f->prev = s;

		// swap the next of f, s
		f->next = s->next;
		s->next = f;

		// move to next 2 node
		f = f->next;
		if (f)
			s = f->next;

	}
}


int main()
{
	double_linked_list_node_t * head = NULL;

	head = dlist_add(head, 1);
	head = dlist_add(head, 2);
	head = dlist_add(head, 3);
	head = dlist_add(head, 4);
	head = dlist_add(head, 5);
	dlist_traversal(head);

	//dlist_remove(&head, remove_fn_, 1);
	//dlist_traversal(head);

	dlist_reversion(&head);
	dlist_traversal(head);

	return 0;
}

