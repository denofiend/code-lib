/*************************************************************************
        > File Name: B.cpp
      > Author: DenoFiend
      > Mail: denofiend@gmail.com
      > Created Time: 2013年03月01日 星期五 15时00分04秒
 ************************************************************************/

#include<iostream>
#include <algorithm>

using namespace std;

#define OUT(x) (cout << #x << " = " << x << endl)

template<class T> void out(T x, int n){ for (int i = 0; i < n; ++i) std::cout << x[i] << ' '; std::cout << endl; }


int cmp(int a, int b){
	int div_a = a / 3;
	int div_b = b / 3;
	if (div_a == div_b) {
		return (a % 3) > (b % 3);
	}else{
		return  div_a > div_b;
	}
}

int num[110];

int main(){

	int T, S, N, p;
	cin >> T;

	
	for (int i = 1; i <= T; ++i) {

		cin >> N >> S >> p;

		for (int j = 0; j < N; ++j) cin >> num[j];

		//out(num, N);
		sort(num, num + N, cmp);
		//out(num, N);


		int ans = 0;

		//cout << "-----------------------" << endl;
		for (int j = 0; j < N; ++j) {

			int div_3 = num[j] / 3;

		//	cout << "================" << endl;
		//	OUT(num[j]);
		//	OUT(div_3);
			//OUT(p);
			//OUT(S);
		
			if (div_3 >= p || ((p > div_3) && (num[j] % 3) >= (p - div_3))) {
			
				if (2 == p - div_3 && 0 == S) break;
				if (2 == p - div_3) S--;

				ans++;

			}
			else if ( (1 == (p-div_3) && div_3 >= 1)  || (div_3 >= 1 && ((num[j] % 3) == 2) && (p-div_3) == 2)) {

				if (S)
				{
					S--;
					ans++;
				}

			}
			else{
				break;
			}
		}
		cout << "Case #" << i << ": " << ans << endl;
	}
	return 0;
}



