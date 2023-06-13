#include "ra/kernel.hpp"
#include <iostream>
#define show ra::geometry::Kernel<float>::printstat();

using point = typename ra::geometry::Kernel<float>::Point;

int main(){
	ra::geometry::Kernel<float> geokernel;
	ra::geometry::Kernel<float>::Point p1(0,0);
	ra::geometry::Kernel<float>::Point p2(5,5);
	ra::geometry::Kernel<float>::Point p3(2,2);
	ra::geometry::Kernel<float>::Orientation line_ori = geokernel.orientation(p1, p2, p3);
	std::cout << static_cast<int>(line_ori) << '\n';
	show;
	point c1(1, 1);
	point c2(-1, 1);
	point c3(-1, -1);
	point c4(-0.675894, 0.1115478);
	ra::geometry::Kernel<float>::Oriented_side circ_ori = geokernel.side_of_oriented_circle(c1, c2, c3, c4 );
	std::cout << static_cast<int>(circ_ori) << '\n';
	show;	
	return 0;
}
