#ifndef triangulation_2_hpp
#define triangulation_2_hpp

/*
Copyright (c) 2018 Michael Adams

Two-Dimensional Triangulation Class Template Based on
Halfedge Data Structure

Version: 2018-06-03

This code is intended to provide a simple halfedge-based triangulation
class template that can be used for the purposes of Assignment 3 in
SENG 475.

The input_off method of Triangulation_2 (as well as the code that
directly/indirectly uses this function) is not intended to be bulletproof
in terms of handling invalid input data.  It is only intended to be good
enough to get the job done for the purposes of this assignment.  For this
reason, care should be taken to provide valid data to the input operation.

*/

////////////////////////////////////////////////////////////////////////////////
// Configuration settings
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGULATION_2_DEBUG_LEVEL
#define TRIANGULATION_2_DEBUG_LEVEL 0
#endif

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cassert>
#include <set>
#include <map>
#include <vector>
#include <exception>
#include <CGAL/Cartesian.h>
#include <CGAL/Filtered_kernel.h>
#include <CGAL/HalfedgeDS_items_2.h>
#include <CGAL/HalfedgeDS_default.h>
#include <CGAL/HalfedgeDS_decorator.h>
#include <CGAL/HalfedgeDS_vertex_base.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace trilib {

////////////////////////////////////////////////////////////////////////////////
// A helper class for the Trangulation_2 class.
// This code is for internal use only and should not be used directly.
// For this reason, this code is deliberately undocumented.
////////////////////////////////////////////////////////////////////////////////

template <class Kernel>
class Make_halfedge_data_structure
{
private:
	template <class Refs, class Traits>
	struct My_vertex : public CGAL::HalfedgeDS_vertex_base<Refs,
	  CGAL::Tag_true, typename Traits::Point>
	{
	};
	template <class Refs>
	struct My_face : public CGAL::HalfedgeDS_face_base<Refs>
	{
	    My_face() {}
	};
	template <class Refs, class Traits>
	struct My_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs>
	{
		typedef Refs HDS;
		typedef typename Refs::Halfedge_handle Halfedge_handle;
		typedef typename Refs::Halfedge_const_handle Halfedge_const_handle;
	    bool is_border_edge() const {
	        return (this->opposite()->is_border() || this->is_border());
	    }
		Halfedge_handle edge()
		{
			Halfedge_handle h = HDS::halfedge_handle(this);
			Halfedge_handle other = h->opposite();
			return (&*h <  &*other) ? h : other;
		}
		Halfedge_const_handle edge() const
		{
			Halfedge_const_handle h = HDS::halfedge_handle(this);
			Halfedge_const_handle other = h->opposite();
			return (&*h <  &*other) ? h : other;
		}
		bool is_triangle() const
		{
			Halfedge_const_handle h = HDS::halfedge_handle(this);
			return h->next() != h && h->next()->next() != h &&
			  h->next()->next()->next() == h;
		}
	};
	struct My_items : public CGAL::HalfedgeDS_items_2
	{
	    template <class Refs, class Traits>
	    struct Face_wrapper {
	        typedef My_face<Refs> Face;
	    };
	    template <class Refs, class Traits>
	    struct Halfedge_wrapper {
	        typedef My_halfedge<Refs, Traits> Halfedge;
	    };
	    template <class Refs, class Traits>
	    struct Vertex_wrapper {
	        typedef My_vertex<Refs, Traits> Vertex;
	    };
	};
	struct My_traits
	{
	    typedef typename Kernel::Point_2  Point;
	};
public:
	using type = CGAL::HalfedgeDS_default<My_traits, My_items>;
};

////////////////////////////////////////////////////////////////////////////////
// The Triangulation_2 class template.
// A triangulation class based on a halfedge data structure.
////////////////////////////////////////////////////////////////////////////////

/*
Template parameters:
K    The geometry kernel to be used by the triangulation
     (e.g., CGAL::Cartesian<double>).
*/

template <typename K>
class Triangulation_2 {
public:

	// The geometry kernel used by the class.
	using Kernel = K;

	// The halfedge data structure used by the class.
	using HDS = typename Make_halfedge_data_structure<Kernel>::type;

	// The point (in 2-D) type.
	// For the interface provided by Point, see:
	// https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Point__2.html
	// Items of interest: x, y, constructors.
	using Point = typename Kernel::Point_2;

	// The vertex type.
	// For the interface provided by Vertex, see:
	// https://doc.cgal.org/latest/Polyhedron/classCGAL_1_1Polyhedron__3_1_1Vertex.html
	// Items of interest: point, halfedge.
	using Vertex = typename HDS::Vertex;

	// The mutating vertex handle type.
	using Vertex_handle = typename HDS::Vertex_handle;

	// The non-mutating vertex handle type.
	using Vertex_const_handle = typename HDS::Vertex_const_handle;

	// The mutating vertex iterator type.
	using Vertex_iterator = typename HDS::Vertex_handle;

	// The non-mutating vertex iterator type.
	using Vertex_const_iterator = typename HDS::Vertex_const_handle;

	// The face type.
	// For the interface provided by Face, see:
	// https://doc.cgal.org/latest/Polyhedron/classCGAL_1_1Polyhedron__3_1_1Facet.html
	// I think that this type might only be helpful for testing.
	using Face = typename HDS::Face;

	// The mutating face handle type.
	using Face_handle = typename HDS::Face_handle;

	// The non-mutating face handle type.
	using Face_const_handle = typename HDS::Face_const_handle;

	// The mutating face iterator type.
	using Face_iterator = typename HDS::Face_handle;

	// The non-mutating face iterator type.
	using Face_const_iterator = typename HDS::Face_const_handle;

	// The halfedge type.
	// For the interface provided by Halfedge, see:
	// https://doc.cgal.org/latest/Polyhedron/classCGAL_1_1Polyhedron__3_1_1Halfedge.html
	// Items of interest: opposite, next, prev, is_border, is_border_edge,
	// facet, next_on_vertex, prev_on_vertex.
	// In addition to the interface presented in the above CGAL documentation,
	// the member function "edge" is provided.  The function deterministically
	// returns one of the halfedges for an edge.
	using Halfedge = typename HDS::Halfedge;

	// The mutating halfedge handle type.
	using Halfedge_handle = typename HDS::Halfedge_handle;

	// The non-mutating halfedge handle type.
	using Halfedge_const_handle = typename HDS::Halfedge_const_handle;

	// The mutating halfedge iterator type.
	using Halfedge_iterator = typename HDS::Halfedge_handle;

	// The non-mutating halfedge iterator type.
	using Halfedge_const_iterator = typename HDS::Halfedge_const_handle;

	/*
	Important note about halfedge iterators:
	A halfedge and its opposite halfedge always appear consecutively
	in the iteration order.
	To iterate over every edge (instead of halfedge), simply skip every
	other halfedge in the halfedge iteration sequence.
	*/

	/*
	Construct a triangulation from an input stream in OFF format.
	The description of a triangulation is read from the input stream in
	(in OFF format) and a triangulation is constructed corresponding to the
	data read.
	Upon failure, an exception is thrown.  The type of the thrown exception is
	either std::exception or an type derived therefrom.
	In cases of invalid input data (not I/O errors), std::abort might be
	called.
	*/
	Triangulation_2(std::istream& in);

	// The triangulation type is not movable.
	Triangulation_2(Triangulation_2&&) = delete;
	Triangulation_2& operator=(Triangulation_2&&) = delete;

	// The triangulation type is not copyable.
	Triangulation_2(const Triangulation_2&) = delete;
	Triangulation_2& operator=(const Triangulation_2&) = delete;

	/*
	Get the number of vertices in the triangulation.
	*/
	int size_of_vertices() const
	  {return hds_.size_of_vertices();}

	/*
	Get the number of faces in the triangulation.
	*/
	int size_of_faces() const
	  {return hds_.size_of_faces();}

	/*
	Get the number of halfedges in the triangulation.
	*/
	int size_of_halfedges() const
	  {return hds_.size_of_halfedges();}

	/*
	Get the number of edges in the triangulation.
	*/
	int size_of_edges() const
	  {return hds_.size_of_halfedges() / 2;}

	/*
	Get a vertex iterator that refers to the first vertex in the triangulation.
	*/
	Vertex_iterator vertices_begin()
	  {return hds_.vertices_begin();}
	Vertex_const_iterator vertices_begin() const
	  {return hds_.vertices_begin();}

	/*
	Get a vertex iterator that refers to the one-past-the-end vertex in the
	triangulation.
	*/
	Vertex_iterator vertices_end()
	  {return hds_.vertices_end();}
	Vertex_const_iterator vertices_end() const
	  {return hds_.vertices_end();}

	/*
	Get a face iterator that refers to the first face in the triangulation.
	*/
	Face_iterator faces_begin()
	  {return hds_.faces_begin();}
	Face_const_iterator faces_begin() const
	  {return hds_.faces_begin();}

	/*
	Get a face iterator that refers to the one-past-the-end face in the
	triangulation.
	*/
	Face_iterator faces_end()
	  {return hds_.faces_end();}
	Face_const_iterator faces_end() const
	  {return hds_.faces_end();}

	/*
	Get a halfedge iterator that refers to the first halfedge in the
	triangulation.
	*/
	Halfedge_iterator halfedges_begin()
	  {return hds_.halfedges_begin();}
	Halfedge_const_iterator halfedges_begin() const
	  {return hds_.halfedges_begin();}

	/*
	Get a halfedge iterator that refers to the one-past-the-end halfedge in the
	triangulation.
	*/
	Halfedge_iterator halfedges_end()
	  {return hds_.halfedges_end();}
	Halfedge_const_iterator halfedges_end() const
	  {return hds_.halfedges_end();}

	/*
	Perform an edge flip.
	The edge associated with the halfedge h is flipped.
	Precondition:
	The edge to be flipped must be flippable (i.e., it must have two incident
	faces whose union form a strictly convex quadrilateral).
	Return value:
	The halfedge h is returned.
	*/
	Halfedge_handle flip_edge(Halfedge_handle h);

	/*
	Read a triangulation from an input stream in OFF format.
	A triangulation is read in OFF format from the input stream in.
	Return value:
	Upon success, true is returned; otherwise, false is returned.
	*/
	bool input_off(std::istream& in);

	/*
	Write a triangulation to an output stream in OFF format.
	The triangulation is written in OFF format to the output stream out.
	Return value:
	Upon success, true is returned; otherwise, false is returned.
	*/
	bool output_off(std::ostream& out) const;

private:

	class Builder;
	friend class Builder;
	HDS hds_;
};

////////////////////////////////////////////////////////////////////////////////
// The Builder class template.
// A helper class for building triangulations.
// This code is for internal use only and should not be used directly.
// For this reason, this code is deliberately undocumented.
////////////////////////////////////////////////////////////////////////////////

template <typename Kernel>
struct Triangulation_2<Kernel>::Builder
{
public:
	using Triangulation = Triangulation_2<Kernel>;
	using Point = Triangulation::Point;
	Builder();
	~Builder();
	Builder(const Builder&) = delete;
	Builder& operator=(const Builder&) = delete;
	void add_vertex(const Point& p);
	void add_face(int va, int vb, int vc);
	bool apply(Triangulation& tri);

private:

	struct Compare_edge
	{
		bool operator()(std::pair<Vertex_handle, Vertex_handle> ea,
		  std::pair<Vertex_handle, Vertex_handle> eb) const
		{
			std::less<Vertex_handle> compare;
			if (compare(ea.second, ea.first)) {
				std::swap(ea.first, ea.second);
			}
			if (compare(eb.second, eb.first)) {
				std::swap(eb.first, eb.second);
			}
			return compare(ea.first, eb.first) ||
			  (!compare(eb.first, ea.first) && compare(ea.second, eb.second));
		}
	};

	typedef std::map<int, Vertex_handle> Vertex_lut;
	typedef std::map<std::pair<Vertex_handle, Vertex_handle>, Halfedge_handle,
	  Compare_edge> Edge_lut;
	typedef std::vector<Face_handle> Face_list;
	typedef std::set<Halfedge_handle> Halfedge_set;

	Halfedge_handle lookup_halfedge(Vertex_handle va, Vertex_handle vb);

	int num_vertices_;
	Vertex_lut vertex_lut_;
	Edge_lut edge_lut_;
	Face_list face_list_;
	Halfedge_set border_halfedges_;
	HDS hds_;

};

template <typename Kernel>
Triangulation_2<Kernel>::Builder::Builder()
{
	num_vertices_ = 0;
}

template <typename Kernel>
Triangulation_2<Kernel>::Builder::~Builder()
{
}

template <typename Kernel>
void Triangulation_2<Kernel>::Builder::add_vertex(const Point& p)
{
#if (TRIANGULATION_2_DEBUG_LEVEL >= 1)
	std::cerr << "adding vertex " << num_vertices_ << " " << p << "\n";
#endif
	Vertex v;
	v.point() = p;
	Vertex_handle vertex = hds_.vertices_push_back(v);
	vertex->set_halfedge(nullptr);
	vertex_lut_.insert(typename Vertex_lut::value_type(num_vertices_, vertex));
	++num_vertices_;
}

template <typename Kernel>
auto Triangulation_2<Kernel>::Builder::lookup_halfedge(Vertex_handle va,
  Vertex_handle vb) -> Halfedge_handle
{
	Halfedge_handle result;
	typename Edge_lut::iterator i = edge_lut_.find(std::pair<Vertex_handle,
	  Vertex_handle>(va, vb));
	if (i == edge_lut_.end()) {
		Halfedge_handle halfedge = hds_.edges_push_back(
		  typename HDS::Halfedge(), typename HDS::Halfedge());
		edge_lut_.insert(typename Edge_lut::value_type(
		  typename Edge_lut::key_type(va, vb), halfedge));
		result = halfedge;
		result->set_vertex(vb);
		result->set_face(nullptr);
		result->set_next(nullptr);
		result->set_prev(nullptr);
		if (vb->halfedge() == typename HDS::Halfedge_handle()) {
			vb->set_halfedge(result);
		}
		border_halfedges_.insert(result);
		result->opposite()->set_vertex(va);
		result->opposite()->set_face(nullptr);
		result->opposite()->set_next(nullptr);
		result->opposite()->set_prev(nullptr);
		border_halfedges_.insert(result->opposite());
		if (va->halfedge() == typename HDS::Halfedge_handle()) {
			va->set_halfedge(result->opposite());
		}
	} else {
		Halfedge_handle halfedge = i->second;
		result = halfedge;
		if (result->vertex() == va) {
			result = result->opposite();
		}
		assert(result->vertex() == vb);
		assert(result->vertex() != typename HDS::Vertex_handle() &&
		  result->opposite()->vertex() != typename HDS::Vertex_handle());
	}
	assert(result->opposite()->vertex() == va && result->vertex() == vb);
	return result;
}

template <typename Kernel>
void Triangulation_2<Kernel>::Builder::add_face(int vai, int vbi, int vci)
{
#if (TRIANGULATION_2_DEBUG_LEVEL >= 1)
	std::cerr << "adding face " << vai << " " << vbi << " " << vci << "\n";
#endif
	assert(vai >= 0 && vai < num_vertices_);
	assert(vbi >= 0 && vbi < num_vertices_);
	assert(vci >= 0 && vci < num_vertices_);
	assert(vertex_lut_.find(vai) != vertex_lut_.end());
	assert(vertex_lut_.find(vbi) != vertex_lut_.end());
	assert(vertex_lut_.find(vci) != vertex_lut_.end());
	Vertex_handle va = vertex_lut_[vai];
	Vertex_handle vb = vertex_lut_[vbi];
	Vertex_handle vc = vertex_lut_[vci];
	assert(va != Vertex_handle());
	assert(vb != Vertex_handle());
	assert(vc != Vertex_handle());
#if (TRIANGULATION_2_DEBUG_LEVEL >= 1)
	std::cerr << "    vertices " << va->point() << " " << vb->point() << " " << vc->point() << "\n";
#endif
	assert(CGAL::orientation(va->point(), vb->point(), vc->point()) ==
	  CGAL::LEFT_TURN);

	Halfedge_handle ab = lookup_halfedge(va, vb);
	Halfedge_handle bc = lookup_halfedge(vb, vc);
	Halfedge_handle ca = lookup_halfedge(vc, va);
	assert(ab != Halfedge_handle());
	assert(bc != Halfedge_handle());
	assert(ca != Halfedge_handle());
	assert(ab->opposite()->vertex() == va && ab->vertex() == vb);
	assert(bc->opposite()->vertex() == vb && bc->vertex() == vc);
	assert(ca->opposite()->vertex() == vc && ca->vertex() == va);
	if (ab->is_border() && bc->is_border() && ca->is_border()) {
		Face f;
		Face_handle face = hds_.faces_push_back(f);
		face_list_.push_back(face);
		assert(ab->opposite()->vertex() == va && ab->vertex() == vb);
		ab->set_next(bc);
		ab->set_prev(ca);
		ab->set_face(face);
		assert(bc->opposite()->vertex() == vb && bc->vertex() == vc);
		bc->set_next(ca);
		bc->set_prev(ab);
		bc->set_face(face);
		assert(ca->opposite()->vertex() == vc && ca->vertex() == va);
		ca->set_next(ab);
		ca->set_prev(bc);
		ca->set_face(face);
		face->set_halfedge(ab);
		assert(border_halfedges_.find(ab) != border_halfedges_.end());
		assert(border_halfedges_.find(bc) != border_halfedges_.end());
		assert(border_halfedges_.find(ca) != border_halfedges_.end());
		border_halfedges_.erase(border_halfedges_.find(ab));
		border_halfedges_.erase(border_halfedges_.find(bc));
		border_halfedges_.erase(border_halfedges_.find(ca));
	} else {
		// ERROR
		assert(false);
		abort();
	}
}

template <typename Kernel>
bool Triangulation_2<Kernel>::Builder::apply(Triangulation_2& tri)
{
#if (TRIANGULATION_2_DEBUG_LEVEL >= 1)
	std::cerr << "apply\n";
#endif
	constexpr bool report_all = true;

	Halfedge_handle border_halfedge = Halfedge_handle();

	bool valid = true;

	// Check for any edge that has no incident faces.
	if (valid) {
		for (auto e = edge_lut_.begin(); e != edge_lut_.end(); ++e) {
			if (e->second->is_border() &&
			  e->second->opposite()->is_border()) {
				std::cerr << "edge with no incident faces\n";
				valid = false;
				if (!report_all) {
					break;
				}
			}
		}
	}

	// Check for any vertex that has no incident edges.
	if (valid) {
		typename Vertex_lut::iterator cur;
		typename Vertex_lut::iterator next;
		cur = vertex_lut_.begin();
		while (cur != vertex_lut_.end()) {
			Vertex_handle vertex = cur->second;
			next = cur;
			++next;
			if (vertex->halfedge() == Halfedge_handle()) {
				std::cerr << "vertex with no incident edges " <<
				  vertex->point() << "\n";
				valid = false;
				if (!report_all) {
					break;
				}
			}
			cur = next;
		}
	}

	border_halfedge = *border_halfedges_.begin();

	if (valid) {
		Halfedge_handle cur_halfedge = border_halfedge;
		Halfedge_handle next_halfedge;
		do {
			Halfedge_handle h = cur_halfedge;
			do {
				h = h->opposite()->prev();
			} while (!h->is_border_edge());
			if (h == border_halfedge) {
				// This should not happen.
				assert(false);
				abort();
			}
			next_halfedge = h->opposite();
			next_halfedge->set_prev(cur_halfedge);
			cur_halfedge->set_next(next_halfedge);
			border_halfedges_.erase(border_halfedges_.find(cur_halfedge));
			cur_halfedge = next_halfedge;
		} while (cur_halfedge != border_halfedge);
		// Check for more than one bounding loop.
		if (!border_halfedges_.empty()) {
			std::cerr << "one or more holes are present\n";
			valid = false;
		}
	}

	// Check orientation of finite faces.
	if (valid) {
		CGAL::Orientation orient;
		for (auto i = face_list_.begin(); i != face_list_.end(); ++i) {
			Halfedge_handle halfedge = (*i)->halfedge();
			if ((orient = CGAL::orientation(halfedge->vertex()->point(),
			  halfedge->next()->vertex()->point(),
			  halfedge->next()->next()->vertex()->point())) !=
			  CGAL::LEFT_TURN) {
				std::cerr << "face has incorrect orientation "
				  << halfedge->vertex()->point() << " "
				  << halfedge->next()->vertex()->point() << " "
				  << halfedge->next()->next()->vertex()->point() << " "
				  << orient << "\n";
				valid = false;
				if (!report_all) {
					break;
				}
			}
		}
	}

	// Check orientation of infinite face.
	if (valid) {
		Halfedge_handle cur = border_halfedge;
		do {
			if (CGAL::orientation(cur->prev()->vertex()->point(),
			  cur->vertex()->point(), cur->next()->vertex()->point()) ==
			  CGAL::LEFT_TURN) {
				std::cerr << "border is not convex hull "
				  << cur->prev()->vertex()->point()
				  << " " << cur->vertex()->point()
				  << " " << cur->next()->vertex()->point() << "\n";
				valid = false;
				if (!report_all) {
					break;
				}
			}
			cur = cur->next();
		} while (cur != border_halfedge);
	}

	if (valid) {
		tri.hds_ = std::move(hds_);
	}

	return valid;
}

////////////////////////////////////////////////////////////////////////////////
// Code for Triangulation_2 class.
////////////////////////////////////////////////////////////////////////////////

template <typename Kernel>
Triangulation_2<Kernel>::Triangulation_2(std::istream& in)
{
	hds_.clear();
	if (!input_off(in)) {
		throw std::exception();
	}
}

template <typename Kernel>
bool Triangulation_2<Kernel>::input_off(std::istream& in)
{
	hds_.clear();
	Triangulation_2::Builder builder;
	std::string signature;
	if (!(in >> signature) || signature != "OFF") {
		std::cerr << "not OFF format\n";
		return false;
	}
	int num_vertices;
	int num_faces;
	int num_edges;
	if (!(in >> num_vertices >> num_faces >> num_edges)) {
		std::cerr << "cannot get number of vertices/faces/edges\n";
		return false;
	}
	std::vector<Vertex_handle> v_lut;
	for (int i = 0; i < num_vertices; ++i) {
		Vertex v;
		double x;
		double y;
		double z;
		if (!(in >> x >> y >> z)) {
			std::cerr << "cannot get vertex\n";
			return false;
		}
		builder.add_vertex(Point(x, y));
	}
	for (int i = 0; i < num_faces; ++i) {
		Face f;
		int degree;
		int vi[3];
		if (!(in >> degree >> vi[0] >> vi[1] >> vi[2])) {
			std::cerr << "cannot get face\n";
			return false;
		}
		if (degree != 3) {
			std::cerr << "not a triangle\n";
			return false;
		}
		builder.add_face(vi[0], vi[1], vi[2]);
	}
	if (!builder.apply(*this)) {
		return false;
	}
#if (TRIANGULATION_2_DEBUG_LEVEL >= 1)
	std::cerr << "number of vertices " << hds_.size_of_vertices() << '\n';
	std::cerr << "number of faces " << hds_.size_of_faces() << '\n';
	std::cerr << "number of halfedges " << hds_.size_of_halfedges() << '\n';
#endif
	return true;
}

template <typename Kernel>
bool Triangulation_2<Kernel>::output_off(std::ostream& out) const
{
	out << "OFF\n";
	out << hds_.size_of_vertices() << " " << hds_.size_of_faces() << " "
	  << 0 << "\n";
	std::map<Vertex_const_handle, int> vertex_lut;
	int i = 0;
	for (auto vi = hds_.vertices_begin(); vi != hds_.vertices_end(); ++vi) {
		vertex_lut[&*vi] = i;
		++i;
		out << vi->point().x() << " " << vi->point().y() << " 0\n";
	}
	for (auto fi = hds_.faces_begin(); fi != hds_.faces_end(); ++fi) {
		Halfedge_const_handle h = fi->halfedge();
		int v0 = vertex_lut[h->vertex()];
		h = h->next();
		int v1 = vertex_lut[h->vertex()];
		h = h->next();
		int v2 = vertex_lut[h->vertex()];
		out << "3 " << v0 << " " << v1 << " " << v2 << "\n";
	}
	return bool(out);
}

template <typename Kernel>
auto Triangulation_2<Kernel>::flip_edge(Halfedge_handle h) -> Halfedge_handle
{
	CGAL::HalfedgeDS_items_decorator<HDS> decorator;
	//std::cerr << "flipping edge\n";
	auto result = decorator.flip_edge(h);
	assert(result == h);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}

#endif
