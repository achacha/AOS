#include <iostream>

template< double R > struct tmplSin
{
	static inline double sin()
	{
		// 1 - R2 / (t*2 * t*2+1)
		double R2 = R * R;
		return R	* ( 1.0 - R2 / (2.0 * 3.0)
							* ( 1.0 - R2 / (4.0 * 5.0)
							* ( 1.0 - R2 / (6.0 * 7.0)
							* ( 1.0 - R2 / (8.0 * 9.0)
							* ( 1.0 - R2 / (10.0 * 11.0)
							* ( 1.0 - R2 / (12.0 * 13.0)
							* ( 1.0 - R2 / (14.0 * 15.0)
							* ( 1.0 - R2 / (16.0 * 17.0)
							* ( 1.0 - R2 / (18.0 * 19.0)
							* ( 1.0 - R2 / (20.0 * 21.0)
							))))))))));
	}
};

#define mt_Sin( r ) tmplSin< r >::sin();

int main()
{
	double x = mt_Sin(3.14159654);
	std::cout << "Factorial(Pi)=" << x << std::endl;
	x = mt_Sin(1.570796327);
	std::cout << "Factorial(Pi)=" << x << std::endl;
	return 0;
}
