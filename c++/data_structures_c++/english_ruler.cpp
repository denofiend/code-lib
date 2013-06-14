/*************************************************************************
        > File Name: english_ruler.cpp
      > Author: DenoFiend
      > Mail: denofiend@gmail.com
      > Created Time: 2013年06月14日 星期五 14时45分36秒
 ************************************************************************/

#include<iostream>
using namespace std;

void printTicks(int ticks, int num = -1);

void printTicks(int ticks, int num){
	for (int i = 0; i < ticks; ++i)
		printf("-");

	if (-1 != num)
		printf(" %d", num);

	printf("\n");
}

void recursion(int ticks){
	if (0 >= ticks) return;
	if (1 == ticks){
		printTicks(1);
		return;
	}

	// ll
	recursion(ticks-1);

	// mid
	printTicks(ticks);

	// rr
	recursion(ticks-1);
}

void english_ruler(int ticks, int inch){

	printTicks(ticks, 0);	

	for (int i = 1; i <= inch; ++i) {
		recursion(ticks-1);
		printTicks(ticks, i);
	}
	
}


int main(){
	int ticks, inch;
	while (cin >> ticks >> inch)
	{
		if (0 == ticks && 0 == inch) break;
		english_ruler(ticks, inch);

	}
	return 0;
}

