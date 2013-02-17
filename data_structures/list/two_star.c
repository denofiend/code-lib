/*************************************************************************
  > File Name: two_star.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月17日 星期日 10时57分07秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct node
{
	struct node * next;
	char data;
} node;

typedef bool (* remove_fn)(node const * v);

// Remove all nodes from the supplied list for which the
// supplied remove function returns true.
// Returns the new head of the list.
node * remove_if(node * head, remove_fn rm)
{
	printf(">>> remove_if [%p]\n", head);

	for (node * prev = NULL, * curr = head; curr != NULL; )
	{
		node * const next = curr->next;
		if (rm(curr))
		{
			if (prev)
				prev->next = next;
			else
				head = next;
			free(curr);
		}
		else
			prev = curr;
		curr = next;
	}
	return head;
}

void remove_if_two_star(node ** head, remove_fn rm)
{
	for (node** curr = head; *curr; )
	{
		node * entry = *curr;
		if (rm(entry))
		{
			*curr = entry->next;
			free(entry);
		}
		else
			curr = &entry->next;
	}
}

bool  remove_fn_(node const*v){
	return v->data == 'a';
}

node* create_list(char*data, int n)
{
	printf(">>> create_list(%s)\n", data);
	node* head = NULL;
	node* now;
	int i;

	for (i = 0; i < n; ++i)
	{
		now = (node*) malloc(sizeof(node));
		now->data = data[i];

		printf("[%p %c]\n", now, now->data);

		if (NULL == head)
			head = now;
		else
		{
			now->next = head->next;
			head->next = now;
		}
	}

	return head;
}

void print_list(node* head)
{
	printf(">>>print_list %p\n", head);

	node * p = head;
	while (p)
	{
		printf("[%p, %p, %c]\n", p, p->next, p->data);
		p = p->next;
	}
	printf("\n");
}

int main()
{
	node *head;
	char data[] = "abcdef";

	head = create_list(data, 6);
	print_list(head);

	//head = remove_if(head,  remove_fn_);
	remove_if_two_star(&head,  remove_fn_);
	print_list(head);


	return 0;
}

