/*************************************************************************
  > File Name: speaking_in_tongues.c
  > Author: DenoFiend
  > Mail: denofiend@gmail.com
  > Created Time: 2013年02月26日 星期二 11时33分09秒
 ************************************************************************/

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <fstream>
#include <numeric>
#include <iomanip>
#include <bitset>
#include <list>
#include <stdexcept>
#include <functional>
#include <utility>
#include <ctime>

using namespace std;

char tongues[27] = "ynficwlbkuomxsevzpdrjgthaq";

char a1[] = "ejp mysljylc kd kxveddknmc re jsicpdrysi";
char a2[] = "our language is impossible to understand";
char b1[] = "rbcpc ypc rtcsra dkh wyfrepkym veddknkmkrkcd";
char b2[] = "there are twenty six factorial possibilities";
char c1[] = "de kr kd eoya kw aej tysr re ujdr lkgc jv";
char c2[] = "so it is okay if you want to just give up";


void sovleCase(const char*a1, const char* a2){
	int len1, len2, i;
	len1 = strlen(a1);
	len2 = strlen(a2);

	if (len1 != len2){
		printf("[ERROR] len1 not eqaul len2\n");
		return;
	}


	for (i = 0; i < len2; ++i)
	{
		tongues[a1[i]-'a'] = a2[i];
	}

}
void initTongues(){
	int i;
	char c;

	memset(tongues, 0, sizeof(tongues));

	tongues['q' - 'a'] = 'z';

	sovleCase(a1, a2);
	sovleCase(b1, b2);
	sovleCase(c1, c2);


	
	/*
	for (i = 0; i < 26; ++i)
		printf ("%c - > %c\n", ('a' + i), tongues[i]);
		*/



	for (c = 'a'; c <= 'z'; ++c)
	{
		i = 0;
		for (; i < 26; ++i)
			if (tongues[i] == c)
				break;
		//printf("[%d %c]\n", i, c);

		if (i == 26)
			break;
	}

	for (i = 0; i < 26; ++i)
		if (tongues[i] == '\0')
			tongues[i] = c;

}

int main(){
	int T, i;

	freopen("A.in","r",stdin);
	freopen("out.txt","w",stdout);

	cin >> T;

	initTongues();
	
	for (i = 0; i <= T; ++i)
	{
		std::string input;
		getline(cin, input);

		if (0 == i) continue;

		std::cout << "Case #" << i << ": ";

		for (int i = 0; i < (int)input.size(); ++i)
		{
			if (' ' == input[i])
				std::cout << ' ';
			else if ('a' <= input[i] && input[i] <= 'z')
				std::cout << tongues[input[i]-'a'];

		}
		std::cout << endl;

	}

	return 0;
}

