/*************************************************************************
  > File Name: C.cpp
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年03月01日 星期五 16时39分25秒
 ************************************************************************/

#include <iostream>
#include <string.h>
using namespace std;


#define OUT(x) (cout << #x << " = " << x << endl)

#define MAXN 2000001

typedef struct hash_node{
	hash_node *next;
	int val;
}hash_node_t;

hash_node_t hash[MAXN];

bool hash_find(int n, int m){
	hash_node_t *head = hash[n].next;

	while (head)
	{
		if (head->val == m) return true;
		head = head->next;
	}

	return false;
}


bool hash_add(int n, int m) {
	hash_node_t * now = new hash_node_t;
	if (NULL == now)
	{
		return false;
	}
	now->val = m;

	now->next = hash[n].next;
	hash[n].next = now;

	return true;
}

int cal(int sta, int end) {

	int ans = 0;
	memset(hash, 0, sizeof(hash));

	for (int n = sta; n <= end; ++n) {

		int digit_pre = 1, digit_bak = 1;
		int i = n, m;

		while (i) 
		{
			digit_pre *= 10;
			i /= 10;
		}

		i = n / 10;
		digit_bak = 10;
		digit_pre /= 10;

		while (i)
		{
			m = n % digit_bak;
			m = m * digit_pre + n / digit_bak;	

			i /= 10;
			digit_pre /= 10;
			digit_bak *= 10;

			if (n >= sta && m > n && m <= end && !hash_find(n, m)) {

				ans++;
				hash_add(n, m);
			}
		}
	}

	return ans;
}

int main(){

	int T, A, B;

	cin >> T;

	for (int i = 1; i <= T; ++i) {
		cin >> A >> B;
		cout << "Case #" << i << ": " << cal(A, B) << endl; 
	}

	return 0;
}

