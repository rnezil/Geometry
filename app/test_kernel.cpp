#include "ra/kernel.hpp"
#include <iostream>
#define show ra::geometry::Kernel<float>::printstat();

using point = typename ra::geometry::Kernel<float>::Point;
using vec = typename ra::geometry::Kernel<float>::Vector;

int main(){
	ra::geometry::Kernel<float> geokernel;
	ra::geometry::Kernel<float>::Point p1(0,0);
	ra::geometry::Kernel<float>::Point p2(5,5);
	ra::geometry::Kernel<float>::Point p3(2.69,2.42);
	ra::geometry::Kernel<float>::Orientation line_ori = geokernel.orientation(p1, p2, p3);
	std::cout << static_cast<int>(line_ori) << '\n';
	show;
	point c1(1, 1);
	point c2(-1, 1);
	point c3(-1, -1);
	point c4(1, -1);
	ra::geometry::Kernel<float>::Oriented_side circ_ori = geokernel.side_of_oriented_circle(c1, c2, c3, c4 );
	std::cout << static_cast<int>(circ_ori) << '\n';
	show;	
	point a(0,0);
	point b(5,1);
	point c(4,-1);
	point d(0,6);
	vec v(-1,1);
	std::cout << geokernel.preferred_direction(a,b,c,d,v) << '\n';
	show;
	point sq1(0,0);
	point sq2(7,0);
	point sq3(7,7);
	point sq4(0,7);
	std::cout << "Is a square strictly convex? ";
	std::cout << geokernel.is_strictly_convex_quad(sq1,sq2,sq3,sq4);
	std::cout << '\n';
	std::cout << "Is an hourglass strictly convex? ";
	std::cout << geokernel.is_strictly_convex_quad(sq1,sq3,sq2,sq4);
	std::cout <<'\n';
	show;
	point q1(5,2);
	point q2(0,1);
	point q3(5,0);
	point q4(10,1);
	point q5(5.15,1);
	std::cout << "Is a diamond locally Delaunay? ";
	std::cout << geokernel.is_locally_delaunay_edge(q1,q2,q3,q4);
	std::cout << '\n';
	std::cout << "Is an arrowhead locally Delaunay? ";
	std::cout << geokernel.is_locally_delaunay_edge(q1,q2,q3,q5);
	std::cout << '\n';
	show;
	ra::geometry::Kernel<float>::Statistics stats;
	ra::geometry::Kernel<float>::get_statistics(stats);
	std::cout << "Copied statistics:\n\n";
	std::cout << '\n';
		std::cout << "Orientation total count:\t\t" << stats.orientation_total_count << '\n';
		std::cout << "Orientation exact count:\t\t" << stats.orientation_exact_count << '\n';
		std::cout << "Preferred direction total count:\t" << stats.preferred_direction_total_count << '\n';
		std::cout << "Preferred direction exact count:\t" << stats.preferred_direction_exact_count << '\n';
		std::cout << "Side of oriented circle total count:\t" << stats.side_of_oriented_circle_total_count << '\n';
		std::cout << "Side of oriented circle exact count:\t" << stats.side_of_oriented_circle_exact_count << '\n';
		std::cout << '\n';
	ra::geometry::Kernel<float>::clear_statistics();
	std::cout << "Cleared statistics.\n\n";
	show;
	return 0;
}
