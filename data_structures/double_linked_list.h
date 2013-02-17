/*************************************************************************
  > File Name: double_linked_list.h
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月17日 星期日 17时22分23秒
 ************************************************************************/

#ifndef DOUBLE_LINKED_LIST_H_
#define DOUBLE_LINKED_LIST_H_

typedef struct double_linked_list_node_s double_linked_list_node_t;

struct double_linked_list_node_s{
	double_linked_list_node_t *prev;
	double_linked_list_node_t *next;
	int data;
};

typedef int (* remove_fn) (double_linked_list_node_t const * v, int val);

/* add one node that data is val into list*/
double_linked_list_node_t* dlist_add(double_linked_list_node_t* head, int val);

/* remove all node in list, who data is val */
void dlist_remove(double_linked_list_node_t** head, remove_fn rm, int val);

/* find the val the list, then change data to expect_val.*/
double_linked_list_node_t* dlist_modify(double_linked_list_node_t* head, int val, int expect_val);

/* traversal the list, print into stdout */
void dlist_traversal(double_linked_list_node_t* head);

#endif

