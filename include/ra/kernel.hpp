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

	// The type used to perform interval arithmetic.
	using Interval = ra::math::interval<R>;

	// The type used to perform exact arithmetic.
	using Exact = CGAL::MP_Float;

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

	// Useful member functions begin here.

	Orientation orientation( const Point& a, const Point& b, const Point& c ){
		//line pointing from a to b: turn left to see c? turn right to see c? collinear?
		
		// Convert each point to singleton intervals
		Interval ax(a.x());
		Interval ay(a.y());
		Interval bx(b.x());
		Interval by(b.y());
		Interval cx(c.x());
		Interval cy(c.y());
		
		// Calculate determinant
		Interval result = (ax - cx) * (by - cy) - (ay - cy) * (bx - cx);
		
		// Record orientation test
		did_orientation();

		try{
			return Orientation(result.sign());
		}catch( ra::math::indeterminate_result& e ){
			// Collect values for exact arithmetic
			Exact ax(a.x());
			Exact ay(a.y());
			Exact bx(b.x());
			Exact by(b.y());
			Exact cx(c.x());
			Exact cy(c.y());

			// Calculate determinant
			Exact result = ((ax - cx) * (by - cy)) - ((ay - cy) * (bx - cx));

			// Record exact orientation test
			did_exact_orientation();

			// Conditional return
			if( result > Exact(0) ){
				return Orientation(1);
			}else if( result < Exact(0) ){
				return Orientation(-1);
			}else{
				return Orientation(0);
			}
		}
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

	static void did_orientation(){ ++(statistics_.orientation_total_count); }
	static void did_exact_orientation(){ ++(statistics_.orientation_exact_count); }
	static void did_preferred_direction(){ ++(statistics_.preferred_direction_total_count); }
	static void did_exact_preferred_direction(){ ++(statistics_.preferred_direction_exact_count); }
	static void did_side_of_oriented_circle(){ ++(statistics_.side_of_oriented_circle_total_count); }
	static void did_exact_side_of_oriented_circle(){ ++(statistics_.side_of_oriented_circle_exact_count); }
};
}
