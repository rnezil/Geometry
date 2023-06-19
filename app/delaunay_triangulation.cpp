#include "triangulation_2.hpp"
#include "ra/kernel.hpp"
#include <iostream>
#include <CGAL/Cartesian.h>
#include <queue>
#include <unordered_set>

using Kernel = ra::geometry::Kernel<double>;

using Triangulation = trilib::Triangulation_2<Kernel>;

using Halfedge = Triangulation::Halfedge_handle;

using Vertex = Triangulation::Vertex_handle;

int main() {
	Kernel predicator;
	Triangulation trangle(std::cin);

	std::unordered_set<Halfedge> true_optimals {};
	std::unordered_set<Halfedge> optimals {};
	std::queue<Halfedge> sus {};
	for(auto iter = trangle.halfedges_begin(); iter != trangle.halfedges_end(); ++iter){
		Halfedge h = &*iter;
		if( h->is_border() || !predicator.is_strictly_convex_quad(
				h->vertex()->point(),
				h->next()->vertex()->point(),
				h->opposite()->vertex()->point(),
				h->opposite()->next()->vertex()->point()) ){
			// If edge is unflippable put it in true_optimals set
			// because its optimality can never change regardless
			// of future edge flips.
			true_optimals.insert(h);
		}else{
			// Place all flippable edges in sus queue.
			sus.push(h);
		}
	}
	
	// Initialize vectors for preferred direction delaunay testing.
	CGAL::Cartesian<double>::Vector_2 u(1,0);
	CGAL::Cartesian<double>::Vector_2 v(1,1);

	// Iterate over sus queue until empty.
	while( !sus.empty() ) {
		if( optimals.find(sus.front()) != optimals.end() ){
			sus.pop();
		}else if( !predicator.is_locally_pd_delaunay_edge(
				sus.front()->opposite()->vertex()->point(),
				sus.front()->opposite()->next()->vertex()->point(),
				sus.front()->vertex()->point(),
				sus.front()->next()->vertex()->point(),
				u,
				v ) ) {
			// If edge fails locally preferred delaunay test,
			// flip edge, place edge in optimals set.
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
			optimals.insert(sus.front());
			optimals.insert(sus.front()->opposite());
			sus.pop();
		}
	}

	trangle.output_off(std::cout);
	return 0;
}
