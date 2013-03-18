/*************************************************************************
        > File Name: b.cpp
      > Author: DenoFiend
      > Mail: denofiend@gmail.com
      > Created Time: 2013年01月05日 星期六 10时35分16秒
 ************************************************************************/

#include<iostream>
using namespace std;

struct t
{
	bool a;
	int b;
	bool c;
};
struct t1
{
	bool a;
	short b;
	bool c;
};
struct t2
{
	bool a;
	char b;
	bool c;
};
int main()
{
	cout << sizeof(t) << endl << sizeof(t1) << endl << sizeof(t2) << endl;

	return 0;
}

