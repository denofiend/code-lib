/*************************************************************************
        > File Name: diff_max.c
      > Author: DenoFiend
      > Mail: denofiend@gmail.com
      > Created Time: 2013年08月07日 星期三 14时26分51秒
 ************************************************************************/
/* 
 * 给定无序数组A，在线性时间内找到i和j，j>i,并且保证A[j]-A[i]是最大的。
 * */

#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int a[100], f[100];

int main() {
	int n, i, j, mm, tmp, k;
	while (EOF != scanf("%d", &n)) {

		if (0 == n || 1 == n) {
			printf(" no ans \n");
			break;
		}

		memset(a, 0, sizeof(a));
		memset(f, -1, sizeof(f));
		for (i = 0; i < n; ++i) scanf("%d", &a[i]);

		f[0] = 0;
		for (i = 1; i < n; ++i) {
			if (a[i] < a[f[i-1]]) {
				f[i] = i;
			} else {
				f[i] = f[i-1];
			}
		}

		i = 0;
		j = 1;
		mm = a[j] - a[i];
		for (k = 1; k < n; ++k) {
			tmp = a[k] - a[ f[k-1] ];

			if (tmp > mm) {
				i = f[k-1];
				j = k;
				mm = tmp;
			}
		}

		printf("i = %d, j = %d, max = %d\n", i, j, mm);

	}
	return 0;
}

