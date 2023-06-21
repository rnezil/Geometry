#include "triangulation_2.hpp"
#include "ra/kernel.hpp"
#include <iostream>
#include <CGAL/Cartesian.h>
#include <queue>
#include <unordered_set>

using Kernel = ra::geometry::Kernel<double>;

using Triangulation = trilib::Triangulation_2<Kernel>;

using Halfedge = Triangulation::Halfedge_handle;

int main() {
	Kernel predicator;
	Triangulation trangle(std::cin);
	
	// Set to containly edges who are currently optimal but
	// whose optimality status is subject to change
	std::unordered_set<Halfedge> optimals {};

	// Queue to contain suspect edges
	std::queue<Halfedge> sus {};

	// Iterate over all halfedges in the triangulation.
	// Test if halfedge corresponds to a border edge; if
	// so, do nothing at border edges are permanently
	// optimal. Otherwise, test if edge is a strictly convex
	// quadrilateral; if not, place into sus queue, otherwise
	// place into optimals set.
	for(auto iter = trangle.halfedges_begin(); iter != trangle.halfedges_end(); ++iter){
		Halfedge h = &*iter;
		if( !(h->is_border_edge()) ){
			if( predicator.is_strictly_convex_quad(
					h->vertex()->point(),
					h->next()->vertex()->point(),
					h->opposite()->vertex()->point(),
					h->opposite()->next()->vertex()->point()) ) {
				sus.push(h);
			}else{
				optimals.insert(h);
			}
		}
	}

	// Ensure all edges are either fully in or fully out of
	// optimals set, i.e. no cases where one halfedge is in
	// and another is out.
	for( auto ha : optimals )
		if( optimals.find(ha->opposite()) != optimals.end() )
			optimals.insert( ha->opposite() );

	// Create vectors for preferred directions delaunay test
	CGAL::Cartesian<double>::Vector_2 u(1,0);
	CGAL::Cartesian<double>::Vector_2 v(1,1);

	// Iterate over sus queue until empty.
	while( !sus.empty() ) {
		if( !predicator.is_locally_pd_delaunay_edge(
				sus.front()->opposite()->vertex()->point(),
				sus.front()->opposite()->next()->vertex()->point(),
				sus.front()->vertex()->point(),
				sus.front()->next()->vertex()->point(),
				u,
				v )
		 && predicator.is_strictly_convex_quad(
				sus.front()->vertex()->point(),
				sus.front()->next()->vertex()->point(),
				sus.front()->opposite()->vertex()->point(),
				sus.front()->opposite()->next()->vertex()->point()) ) {
			// If edge fails locally preferred delaunay test
			// and is part of a strictly convex quadrilateral,
			// then flip edge and place edge in optimals set.
			trangle.flip_edge(sus.front());
			optimals.insert(sus.front());
			optimals.insert(sus.front()->opposite());

			// Place previously optimal edges that might have been
			// affected by the edge flip into the sus queue.
			if( optimals.find(sus.front()->next()) != optimals.end() ){
				sus.push(*optimals.find(sus.front()->next()));
				sus.push(*optimals.find(sus.front()->next()->opposite()));
				optimals.erase(optimals.find(sus.front()->next()));
				optimals.erase(optimals.find(sus.front()->next()->opposite()));
			}

			if( optimals.find(sus.front()->prev()) != optimals.end() ){
				sus.push(*optimals.find(sus.front()->prev()));
				sus.push(*optimals.find(sus.front()->prev()->opposite()));
				optimals.erase(optimals.find(sus.front()->prev()));
				optimals.erase(optimals.find(sus.front()->prev()->opposite()));
			}

			if( optimals.find(sus.front()->opposite()->next()) != optimals.end() ){
				sus.push(*optimals.find(sus.front()->opposite()->next()));
				sus.push(*optimals.find(sus.front()->opposite()->next()->opposite()));
				optimals.erase(optimals.find(sus.front()->opposite()->next()));
				optimals.erase(optimals.find(sus.front()->opposite()->next()->opposite()));
			}

			if( optimals.find(sus.front()->opposite()->prev()) != optimals.end() ){
				sus.push(*optimals.find(sus.front()->opposite()->prev()));
				sus.push(*optimals.find(sus.front()->opposite()->prev()->opposite()));
				optimals.erase(optimals.find(sus.front()->opposite()->prev()));
				optimals.erase(optimals.find(sus.front()->opposite()->prev()->opposite()));
			}
		}else{
			// If edge satisfies preferred directions delaunay test
			// then mark it as optimal.
			optimals.insert( sus.front() );
			optimals.insert( sus.front()->opposite() );
			sus.pop();
		}
	}

	// Output triangulation to stdout in OFF format.
	trangle.output_off(std::cout);
	return std::cout ? 0 : 1;
}
