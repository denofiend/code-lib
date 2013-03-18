/*************************************************************************
  > File Name: a.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年01月05日 星期六 10时22分29秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>

int main()
{
	int n;
	scanf("%d", &n);

	if (n == 1)
	{
		printf("1=1\n");
		return;
	}

	if (n == 2)
	{
		printf("2=1+1\n");
		return;
	}

	int *a = malloc(n);
	int top = 0;
	a[0] = n - 1;
	a[1] = 1;
	top = 2;

	int i;
	do{
		printf("%d=%d", n, a[0]);
		for (i = 1; i < top; i++)
		{
			printf("+%d", a[i]);
		}
		printf("\n");

		int s = 0;
		do{
			s += a[--top];
		}while (top >= 0 && a[top] == 1);

		if (top == -1)
		{
			break;
		}

		//printf ("s=%d\n", s);
		int d = a[top] - 1;
		if (d == 1)
		{
			while (s > 0)
			{
				a[top++] = 1;
				s--;
			}
		}
		else
		{
			do{
				a[top++] = d;
				s -= d;
			}while (s >= d);
			if (s != 0)
			{
				a[top++] = s;
			}
		}
	}while (1);
	return 0;
}

