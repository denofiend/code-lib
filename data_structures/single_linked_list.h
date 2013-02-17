/*************************************************************************
  > File Name: single_linked_list.h
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月17日 星期日 14时22分23秒
 ************************************************************************/

#ifndef SINGLE_LINKED_LIST_H
#define SINGLE_LINKED_LIST_H

typedef struct single_linked_list_node_s single_linked_list_node_t;

struct single_linked_list_node_s{
	single_linked_list_node_t *next;
	int data;
};

typedef int (* remove_fn) (single_linked_list_node_t const * v, int val);

/* add one node that data is val into list*/
single_linked_list_node_t* slist_add(single_linked_list_node_t* head, int val);

/* remove all node in list, who data is val */
void slist_remove(single_linked_list_node_t** head, remove_fn rm, int val);

/* find the val the list, then change data to expect_val.*/
single_linked_list_node_t* slist_modify(single_linked_list_node_t* head, int val, int expect_val);

/* traversal the list, print into stdout */
void slist_traversal(single_linked_list_node_t* head);

#endif

