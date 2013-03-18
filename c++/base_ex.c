/*************************************************************************
        > File Name: base_ex.c
      > Author: DenoFiend
      > Mail: denofiend@gmail.com
      > Created Time: 2013年01月31日 星期四 14时23分27秒
 ************************************************************************/

#include<stdio.h>


int power(int base, int ex)
{
	printf(">>> power(%d %d)\n", base, ex);
	int result;
	if (0 == ex)	return 1;
	else if (1 == ex) return base;
	else
	{
		result = power(base, ex/2);
		result *= result;

		if (1 == ex % 2)
		{
			result *= base;
		}
		return result;
	}
}


int main()
{
	int base, ex;
	printf ("pleas enter base, ex\n");

	while (EOF != scanf("%d %d", &base, &ex))
	{
		printf ("%d^%d = %d\n", base, ex, power(base, ex));
	}

	return 0;
}

