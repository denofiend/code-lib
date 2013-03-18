/*************************************************************************
  > File Name: dp.cpp
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月21日 星期四 11时18分58秒
 ************************************************************************/

#include<iostream>
using namespace std;
#include <string>
#include <map>
#include <set>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

template <class T> void out(T x, int n){    for(int i = 0; i < n; ++i)    cout << x[i] << ' ';    cout << endl;    }
template <class T> void out(T x, int n, int m){    for(int i = 0; i < n; ++i)    out(x[i], m);    cout << endl;    }


#define OUT(x) (cout << #x << " = " << x << endl)
#define FOR(i, a, b)    for(int i = (int)a; i < (int)b; ++i)
#define REP(i, b)    FOR(i, 0, b)
#define FORD(i, a, b)    for(int i = (int)a; i >= (int)b; --i)
#define Inf 0x7fffff
#define MAXN 55


int f0[MAXN][MAXN];
int f1[MAXN][MAXN];
int f4[MAXN];
int f5[MAXN];


int cal(int n, int m){
	if ( (n < 1) || (m < 1)) return 0;
	if ((1 == n) || (1 == m)) return f0[n][m] = 1;
	if (f0[n][m]) return f0[n][m];
	if (n < m)   return f0[n][m] = cal(n, n);
	if (n == m) return f0[n][m] = cal(n, m-1) + 1;
	return f0[n][m] = cal(n, m-1) + cal(n-m, m); 
}

void dp(){
	REP (i, MAXN)
		REP (j, MAXN)
		f0[i][j] = f1[i][j] = 0;

	FOR (i, 1, MAXN)
		FOR (j, 1, MAXN)
		{
			cal(i, j);
		}

	REP (i, MAXN)    f4[i] = f5[i] = 0;    

	FOR (i, 1, MAXN)    f1[i][1] = 1;

	FOR (i, 2, MAXN)
	{
		FOR (j, 2, i)
		{
			FOR (k, 1, j+1)
			{
				f1[i][j] += f1[i-j][k];
			}
		}
		f1[i][i] = 1;
	}
	printf("f1[2][2]=%d\n",f1[2][2]);
	//out(f1, 51, 51);
	f4[0] = 1;
	FOR (i, 1, MAXN)
	{
		if (i & 1)
		{
			REP (j, MAXN)
			{
				if (j >= i)    f4[j] += f4[j-i];
			}
		}
	}
	f5[0] = 1;
	FOR (i, 1, MAXN)
	{
		FORD (j, MAXN-1, i)
		{
			f5[j] += f5[j-i];
		}
	}
}

int main(){
	int n, k;
	dp();
	while (EOF != scanf("%d %d\n", &n, &k))
	{
		printf("%d\n%d\n%d\n%d\n%d\n\n", f0[n][n], f1[n][k], f0[n][k], f4[n], f5[n]);
	}
	return 0;
}


