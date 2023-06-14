#include "ra/interval.hpp"
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <cstddef>

namespace ra::geometry {

template<class R>
class Kernel {
	public:

	// The type used to represent real numbers.
	using Real = R;

	// The type used to represent points in two dimensions.
	using Point = typename CGAL::Cartesian<R>::Point_2;

	// The type used to represent vectors in two dimensions.
	using Vector = typename CGAL::Cartesian<R>::Vector_2;

	// The possible outcomes of an orientation test.
	enum class Orientation : int {
		right_turn = -1,
		collinear = 0,
		left_turn = 1,
	};

	// The possible outcomes of an oriented-side-of test.
	enum class Oriented_side : int {
		on_negative_side = -1,
		on_boundary = 0,
		on_positive_side = 1,
	};

	// The set of statistics maintained by the kernel.
	struct Statistics {
		// The total number of orientation tests.
		std::size_t orientation_total_count;
			
		// The number of orientation tests requiring exact arithmetic.
		std::size_t orientation_exact_count;

		// The total number of preferred-direction tests.
		std::size_t preferred_direction_total_count;

		// The number of preferred-direction tests requiring exact
		// arithmetic.
		std::size_t preferred_direction_exact_count;

		// The total number of side-of-oriented-circle tests.
		std::size_t side_of_oriented_circle_total_count;

		// The number of side-of-oriented-circle tests requiring exact
		// arithmetic.
		std::size_t side_of_oriented_circle_exact_count;
	};

	// Since a kernel object is stateless, construction and destruction are
	// trivial.
	Kernel() = default;
	~Kernel() = default;

	// The kernel type is both movable and copyable; but since it is stateless,
	// both the move and copy operations are trivial.
	Kernel(const Kernel&) = default;
	Kernel& operator=(const Kernel&) = default;
	Kernel(Kernel&&) = default;
	Kernel& operator=(Kernel&&) = default;

	Orientation orientation( const Point& a, const Point& b, const Point& c ){

		// Record orientation
		did_orientation();

		try{
			// Compute result as interval
			Interval result = get_orientation_result<Interval>(a,b,c);
			return Orientation(result.sign());
		}
		catch( ra::math::indeterminate_result& e ){

			// Record exact orientation
			did_exact_orientation();

			// Compute exact result
			Exact result = get_orientation_result<Exact>(a,b,c);
			if( Exact(0) < result ){
				return Orientation(1);
			}else if( result < Exact(0) ){
				return Orientation(-1);
			}else{
				return Orientation(0);
			}
		}
	}

	Oriented_side side_of_oriented_circle( const Point& a, const Point& b, const Point& c, const Point& d ) {

		// Record side of oriented circle test
		did_side_of_oriented_circle();

		try{
			// Compute result as interval
			Interval result = get_side_of_oriented_circle_result<Interval>(a,b,c,d);
			return Oriented_side(result.sign());
		}
		catch( ra::math::indeterminate_result& e ) {

			// Record exact side of oriented circle test
			did_exact_side_of_oriented_circle();

			// Compute exact result
			Exact result = get_side_of_oriented_circle_result<Exact>(a,b,c,d);
			if( Exact(0) < result ){
				return Oriented_side(1);
			}else if( result < Exact(0) ){
				return Oriented_side(-1);
			}else{
				return Oriented_side(0);
			}
		}
	}

	int preferred_direction( const Point& a, const Point& b,
			const Point& c, const Point& d, const Vector& v ) {
		
		// Record preferred direction test
		did_preferred_direction();

		try{
			// Compute result as interval
			Interval result = get_preferred_direction_result<Interval>(a,b,c,d,v);
			return(result.sign());
		}
		catch( ra::math::indeterminate_result& e ){

			// Record exact preferred direction test
			did_exact_preferred_direction();

			// Compute exact result
			Exact result = get_preferred_direction_result<Exact>(a,b,c,d,v);
			if( Exact(0) < result ){
				return 1;
			}else if( result < Exact(0) ){
				return -1;
			}else{
				return 0;
			}
		}
	}

	bool is_strictly_convex_quad( const Point& a, const Point& b,
			const Point& c, const Point& d ) {

		// Precondition: vertices a,b,c,d have distinct values
		// and are given in counterclockwise order

		if( (static_cast<int>(orientation(a,b,c)) == 1)
				&& (static_cast<int>(orientation(b,c,d)) == 1)
				&& (static_cast<int>(orientation(c,d,a)) == 1)
				&& (static_cast<int>(orientation(d,a,b)) == 1) )
		{
			return true;
		}else{
			return false;
		}
	}

	bool is_locally_delaunay_edge( const Point& a, const Point& b,
			const Point& c, const Point& d ) {
		// Precondition: a,b,c,d have dinstinct values and
		// the shape they form is strictly convex

		// if e is not locally Delaunay, then e' is
		// if a,b,c,d are cocircular, e and e' both locally Delaunay

		if( static_cast<int>(side_of_oriented_circle(a,b,c,d)) > 0 ){
			return false;
		}else{
			return true;
		}
	}

	bool is_locally_pd_delaunay_edge( const Point& a, const Point& b,
			const Point& c, const Point& d, const Vector& u,
			const Vector& v ) {

		// Precondition: a,b,c,d have distinct values and
		// the vectors u and v are not zero vectors, and
		// are neither parallel nor orthogonal

		// it is always the case that either e or e' has the
		// preferred direction locally Delaunay property

		if( static_cast<int>(side_of_oriented_circle(a,b,c,d)) < 0 ) 
		{
			return true;
		}

		if( static_cast<int>(side_of_oriented_circle(a,b,c,d)) > 0 )
		{
			return false;
		}

		if( (preferred_direction(c,a,b,d,u) > 0) ||
				( (preferred_direction(c,a,b,d,u) == 0) &&
				  (preferred_direction(c,a,b,d,v) > 0) ) )
		{
			return true;
		}
		else
		{
			return false;
		}

		assert(false);
	}

	static void clear_statistics() {
		statistics_.orientation_total_count = 0;
		statistics_.orientation_exact_count = 0;
		statistics_.preferred_direction_total_count = 0;
		statistics_.preferred_direction_exact_count = 0;
		statistics_.side_of_oriented_circle_total_count = 0;
		statistics_.side_of_oriented_circle_exact_count = 0;
	}

	static void get_statistics( Statistics& statistics ) {
		statistics.orientation_total_count = statistics_.orientation_total_count;
		statistics.orientation_exact_count = statistics_.orientation_exact_count;
		statistics.preferred_direction_total_count = statistics_.preferred_direction_total_count;
		statistics.preferred_direction_exact_count = statistics_.preferred_direction_exact_count;
		statistics.side_of_oriented_circle_total_count = statistics_.side_of_oriented_circle_total_count;
		statistics.side_of_oriented_circle_exact_count = statistics_.side_of_oriented_circle_exact_count;
	}

	static void printstat() {
		std::cout << '\n';
		std::cout << "Orientation total count:\t\t" << statistics_.orientation_total_count << '\n';
		std::cout << "Orientation exact count:\t\t" << statistics_.orientation_exact_count << '\n';
		std::cout << "Preferred direction total count:\t" << statistics_.preferred_direction_total_count << '\n';
		std::cout << "Preferred direction exact count:\t" << statistics_.preferred_direction_exact_count << '\n';
		std::cout << "Side of oriented circle total count:\t" << statistics_.side_of_oriented_circle_total_count << '\n';
		std::cout << "Side of oriented circle exact count:\t" << statistics_.side_of_oriented_circle_exact_count << '\n';
		std::cout << '\n';
	}

	private:
	
	static inline Statistics statistics_ {0,0,0,0,0,0};

	// The type used to perform interval arithmetic.
	using Interval = ra::math::interval<R>;

	// The type used to perform exact arithmetic.
	using Exact = CGAL::MP_Float;

	template<class T>
	T get_orientation_result( const Point& a, const Point& b, const Point& c ) const {
		T ax(a.x());
		T ay(a.y());
		T bx(b.x());
		T by(b.y());
		T cx(c.x());
		T cy(c.y());
		
		// Calculate and return determinant
		return ( (ax - cx) * (by - cy) - (ay - cy) * (bx - cx) );
	}

	template<class T>
	T get_side_of_oriented_circle_result( const Point& a, const Point& b, const Point& c,
			const Point& d ) const {
		T ax(a.x());
		T ay(a.y());
		T az = (ax * ax) + (ay * ay);

		T bx(b.x());
		T by(b.y());
		T bz = (bx * bx) + (by * by);

		T cx(c.x());
		T cy(c.y());
		T cz = (cx * cx) + (cy * cy);

		T dx(d.x());
		T dy(d.y());
		T dz = (dx * dx) + (dy * dy);

		// Compute determinant
		return detfor_orient3d(ax, ay, az, bx, by, bz, cx, cy, cz, dx, dy, dz);
	}
		
	template<class T>
	T get_preferred_direction_result( const Point& a, const Point& b, const Point& c, 
			const Point& d, const Vector& v ) const {
		T ax(a.x());
		T ay(a.y());

		T bx(b.x());
		T by(b.y());

		T cx(c.x());
		T cy(c.y());

		T dx(d.x());
		T dy(d.y());

		T vx(v.x());
		T vy(v.y());

		T atob_mag2 = ((bx - ax) * (bx - ax)) + ((by - ay) * (by - ay));
		T ctod_mag2 = ((dx - cx) * (dx - cx)) + ((dy - cy) * (dy - cy));

		T atob_dotv = ((bx - ax) * vx) + ((by - ay) * vy);
		T ctod_dotv = ((dx - cx) * vx) + ((dy - cy) * vy);

		return ( (ctod_mag2 * atob_dotv * atob_dotv)
			- (atob_mag2 * ctod_dotv * ctod_dotv) );
	}

	template<class T>
	T detfor_orient3d ( const T& ax, const T& ay, const T& az, const T& bx, const T& by,
			const T& bz, const T& cx, const T& cy, const T& cz, const T& dx,
			const T& dy, const T& dz) const {
		return ( ((ax - dx) * (((by - dy) * (cz - dz)) - ((bz - dz) * (cy - dy))))
			- ((bx - dx) * (((ay - dy) * (cz - dz)) - ((az - dz) * (cy - dy))))
			+ ((cx - dx) * (((ay - dy) * (bz - dz)) - ((az - dz) * (by - dy)))) );
	}

	static void did_orientation(){ ++(statistics_.orientation_total_count); }
	static void did_exact_orientation(){ ++(statistics_.orientation_exact_count); }
	static void did_preferred_direction(){ ++(statistics_.preferred_direction_total_count); }
	static void did_exact_preferred_direction(){ ++(statistics_.preferred_direction_exact_count); }
	static void did_side_of_oriented_circle(){ ++(statistics_.side_of_oriented_circle_total_count); }
	static void did_exact_side_of_oriented_circle(){ ++(statistics_.side_of_oriented_circle_exact_count); }
};
}
