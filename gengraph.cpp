#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;


int main(int argc, char * argv[])
{
	int n = atoi(argv[1]);
	if (n < 3) n = 3;
	int ** a = new int * [n];
	for(int k = 0; k < n; ++k)
		a[k] = new int [n]();
	int * b = new int [n];

	for (int k = 0; k < n; ++k)
		b[k] = k;


	srand(time(0));
	int m = n * (n-1) / 2 / 2;
	m = rand() % m + m;
	for(int k = 0; k < m; ++k)
	{
		int i = rand() % n, j = rand() % n;
		while(i == j) j = rand() % n;
		a[i][j] = a[j][i] = rand() % 5 + 1;
		if (b[i] < b[j])
			b[j] = b[i];
		else
			b[i] = b[j];
	}
	for(int i = 0; i < n; ++i)
	{
		if (b[i] == 0) continue;
		if (b[i] == i)
			while(b[i] != 0)
			{
				int j = rand() % n;
				while(b[j]) j = rand() % n;
				a[i][j] = a[j][i] = rand() % 5 + 1;
				if (b[i] < b[j])
					b[j] = b[i];
				else
					b[i] = b[j];
			}
		else if (b[i] < i)
			b[i] = b[b[i]];
		else
			throw "inconsistency error";
	}

	cout << n << endl;
	for(int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
			cout << a[i][j] << " ";
		cout << endl;
	}

/*	cout << endl;
	for (int k = 0; k < n; ++k)
		cout << b[k] << " ";
	cout << endl;
*/	
	delete [] b;
	for(int k = 0; k < n; ++k)
		delete [] a[k];
	delete [] a;
	return 0;
}
