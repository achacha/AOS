#include <iostream>

template< unsigned N > struct tmplFactorial
{
	enum { val = N * tmplFactorial< N-1 >::val };
};
template<> struct tmplFactorial< 1 >
{
	enum { val = 1 };
};

#define mt_Factorial( n ) tmplFactorial< n >::val;

int main()
{
	int x = mt_Factorial(6);
	std::cout << "Factorial(6)=" << x << std::endl;
	return 0;
}
