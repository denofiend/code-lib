/*************************************************************************
  > File Name: test.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月20日 星期三 10时29分06秒
 ************************************************************************/

#include<stdio.h>

//##include "stdafx.h"
#define SQR(X) X*X
int main(int argc, char* argv[])
{
	int a = 10;
	int k = 2;
	int m = 1;
	int tmp;
	tmp = SQR(k+m)/SQR(k+m); 
	a /= tmp;
	printf("%d %d\n", tmp, a);
	return 0;
}

