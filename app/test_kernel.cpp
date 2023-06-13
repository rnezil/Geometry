#include "ra/kernel.hpp"
#include <iostream>
#define show ra::geometry::Kernel<float>::printstat();

int main(){
	ra::geometry::Kernel<float> geokernel;
	ra::geometry::Kernel<float>::Point p1(0,0);
	ra::geometry::Kernel<float>::Point p2(5,5);
	ra::geometry::Kernel<float>::Point p3(2,2);
	ra::geometry::Kernel<float>::Orientation ori = geokernel.orientation(p1, p2, p3);
	std::cout << static_cast<int>(ori) << '\n';
	show;
	return 0;
}
