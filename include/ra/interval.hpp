#include <stdexcept>
#include <cfenv>
#include <cassert>

namespace ra::math{
struct indeterminate_result : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

template<typename T>
class interval {
	public:
		using real_type = T;

		struct statistics {
			// The total number of inderterminate results encountered.
			unsigned long indeterminate_result_count;
			// The total number of interval arithmetic operations.
			unsigned long arithmetic_op_count;
		};

		//Default constructor
		interval( real_type init = real_type(0) ) : lower_ {init}, upper_ {init} {};

		//Two argument constructor
		interval( real_type bot, real_type top ) : lower_ {bot}, upper_ {top} {
			assert( lower_ <= upper_ );
		}

		interval& operator +=( interval& other ) {
			// Store users current rounding mode
			int user_rounding_mode = std::fegetround();

			// Capture values to prevent aliasing
			real_type this_lower = lower();
			real_type this_upper = upper();
			real_type other_lower = other.lower();
			real_type other_upper = other.upper();

			// Perform calculations with appropriate rounding modes
			assert( !std::fesetround(FE_DOWNWARD) );
			lower_ = this_lower + other_lower;
			assert( !std::fesetround(FE_UPWARD) );
			upper_ = this_upper + other_upper;

			// Restore users rounding mode
			assert( !std::fesetround(user_rounding_mode) );
			return *this;
		}

		interval& operator -=( interval& other ) {
			// Store users current rounding mode
			int user_rounding_mode = std::fegetround();

			// Capture values to prevent aliasing
			real_type this_lower = lower();
			real_type this_upper = upper();
			real_type other_lower = other.lower();
			real_type other_upper = other.upper();

			// Perform calculations with appropriate rounding modes
			assert( !std::fesetround(FE_DOWNWARD) );
			lower_ = this_lower - other_upper;
			assert( !std::fesetround(FE_UPWARD) );
			upper_ = this_upper - other_lower;

			// Restore users rounding mode
			assert( !std::fesetround(user_rounding_mode) );
			return *this;
		}

		interval& operator *=( interval& other ) {
			// Store users current rounding mode
			int user_rounding_mode = std::fegetround();

			// Capture values to prevent aliasing
			real_type this_lower = lower();
			real_type this_upper = upper();
			real_type other_lower = other.lower();
			real_type other_upper = other.upper();

			// Create 8-bit lookup table for upcoming switch
			char lookup(0);

			// bit 0: this interval contains values equal to or less than zero
			lookup |= (upper() <= 0);

			// bit 1: this interval contains positive and negative values
			lookup |= ((lower() < 0) && (upper() > 0)) << 1;

			// bit 2: this interval contains values equal to or greater than zero
			lookup |= (lower() >= 0) << 2;

			// bit 3: this interval is pure zero
			lookup |= ((lookup & 0x01) & ((lookup & 0x04) >> 2)) << 3;

			// bit 4: other interval contains values equal to or less than zero
			lookup |= (other.upper() <= 0) << 4;

			// bit 5: other interval contains positive and negative values
			lookup |= ((other.lower() < 0) && (other.upper() > 0)) << 5;

			// bit 6: other interval contains values equal to or greater than zero
			lookup |= (other.lower() >= 0) << 6;

			// bit 7: other interval is pure zero
			lookup |= ((lookup & 0x10) & ((lookup & 0x40) >> 2)) << 3;

			// Perform calculations with appropriate rounding modes
			switch( lookup ) {
				case (char)0x11:
					// both negative
					set_round_down();
					lower_ = this_upper * other_upper;
					set_round_up();
					upper_ = this_lower * other_lower;
				case (char)0x21:
					// this negative other both
					set_round_down();
					lower_ = this_lower * other_upper;
					set_round_up();
					upper_ = this_lower * other_lower;
				case (char)0x41:
					// this negative other positive
					set_round_down();
					lower_ = this_lower * other_upper;
					set_round_up();
					upper_ = this_upper * other_lower;
				case (char)0x12:
					// this both other negative
					set_round_down();
					lower_ = this_upper * other_lower;
					set_round_up();
					upper_ = this_lower * other_lower;
				case (char)0x22:
					// both both
					set_round_down();
					lower_ = (this_lower * other_upper < this_upper * other_lower) ? (this_lower * other_upper) : (this_upper * other_lower);
					set_round_up();
					upper_ = (this_lower * other_lower > this_upper * other_upper) ? (this_lower * other_lower) : (this_upper * other_upper);
				case (char)0x42:
					// this both other positive
					set_round_down();
					lower_ = this_lower * other_upper;
					set_round_up();
					upper_ = this_upper * other_upper;
				case (char)0x14:
					// this positive other negative
					set_round_down();
					lower_ = this_upper * other_lower;
					set_round_up();
					upper_ = this_lower * other_upper;
				case (char)0x24:
					// this positive other both
					set_round_down();
					lower_ = this_upper * other_lower;
					set_round_up();
					upper_ = this_upper * other_upper;
				case (char)0x44:
					// this positive other positive
					set_round_down();
					lower_ = this_lower * other_lower;
					set_round_up();
					upper_ = this_upper * other_upper;
				default:
					lower_ = zero();
					upper_ = zero();
			}
			
			// Restore users rounding mode
			assert( !std::fesetround(user_rounding_mode) );
			return *this;
		}

		real_type lower() const { return lower_bound; }

		real_type upper() const { return upper_bound; }
	private:
		real_type lower_;
		real_type upper_;

		static real_type zero() { return real_type(0); }
		static void set_round_down() { assert( !std::fesetround(FE_DOWNWARD) ); }
		static void set_round_up() { assert( !std::fesetround(FE_UPWARD) ); }
};
}
