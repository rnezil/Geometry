#include <stdexcept>
#include <cfenv>
#include <cassert>
#include <iostream>

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
			set_round_down();
			lower_ = this_lower + other_lower;
			set_round_up();
			upper_ = this_upper + other_upper;

			// Record arithmetic operation
			record_arithmetic_op();

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
			set_round_down();
			lower_ = this_lower - other_upper;
			set_round_up();
			upper_ = this_upper - other_lower;

			// Record arithmetic operation
			record_arithmetic_op();

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
			char lookup = get_mul_lookup_table_for( other );

			// Perform calculations with appropriate rounding modes
			switch( lookup ) {
				case (char)0x11:
					// both negative
					//std::cout << *this << '\n' << other << '\n' << "both negative" << "\n\n";
					set_round_down();
					lower_ = this_upper * other_upper;
					set_round_up();
					upper_ = this_lower * other_lower;
					break;
				case (char)0x21:
					// this negative other both
					//std::cout << *this << '\n' << other << '\n' << "this negative other both" << "\n\n";
					set_round_down();
					lower_ = this_lower * other_upper;
					set_round_up();
					upper_ = this_lower * other_lower;
					break;
				case (char)0x41:
					// this negative other positive
					//std::cout << *this << '\n' << other << '\n' << "this negative other positive" << "\n\n";
					set_round_down();
					lower_ = this_lower * other_upper;
					set_round_up();
					upper_ = this_upper * other_lower;
					break;
				case (char)0x12:
					// this both other negative
					//std::cout << *this << '\n' << other << '\n' << "this both other negative" << "\n\n";
					set_round_down();
					lower_ = this_upper * other_lower;
					set_round_up();
					upper_ = this_lower * other_lower;
					break;
				case (char)0x22:
					// both both
					//std::cout << *this << '\n' << other << '\n' << "both both" << "\n\n";
					set_round_down();
					lower_ = (this_lower * other_upper < this_upper * other_lower) ? (this_lower * other_upper) : (this_upper * other_lower);
					set_round_up();
					upper_ = (this_lower * other_lower > this_upper * other_upper) ? (this_lower * other_lower) : (this_upper * other_upper);
					break;
				case (char)0x42:
					// this both other positive
					//std::cout << *this << '\n' << other << '\n' << "this both other positive" << "\n\n";
					set_round_down();
					lower_ = this_lower * other_upper;
					set_round_up();
					upper_ = this_upper * other_upper;
					break;
				case (char)0x14:
					// this positive other negative
					//std::cout << *this << '\n' << other << '\n' << "this positive other negative" << "\n\n";
					set_round_down();
					lower_ = this_upper * other_lower;
					set_round_up();
					upper_ = this_lower * other_upper;
					break;
				case (char)0x24:
					// this positive other both
					//std::cout << *this << '\n' << other << '\n' << "this positive other both" << "\n\n";
					set_round_down();
					lower_ = this_upper * other_lower;
					set_round_up();
					upper_ = this_upper * other_upper;
					break;
				case (char)0x44:
					// this positive other positive
					//std::cout << *this << '\n' << other << '\n' << "both positive" << "\n\n";
					set_round_down();
					lower_ = this_lower * other_lower;
					set_round_up();
					upper_ = this_upper * other_upper;
					break;
				default:
					//std::cout << *this << '\n' << other << '\n' << "this or other zero" << "\n\n";
					lower_ = zero();
					upper_ = zero();
			}
			
			// Record arithmetic operation
			record_arithmetic_op();

			// Restore users rounding mode
			assert( !std::fesetround(user_rounding_mode) );
			return *this;
		}

		real_type lower() const { return lower_; }

		real_type upper() const { return upper_; }

		bool is_singleton() const { return upper() == lower(); }

		int sign() const {
			if( upper() < 0 ) {
				return -1;
			}else if( lower() > 0 ) {
				return 1;
			}else if( (lower() == 0) && (upper() == 0) ) {
				return 0;
			}else {
				// Record indeterminate result and throw exception
				record_indeterminate_result();
				throw indeterminate_result {"Cannot determine sign of interval"};
			}
		}

		static void clear_statistics() {
			statistics_.indeterminate_result_count = 0;
			statistics_.arithmetic_op_count = 0;
		}

		static void get_statistics( statistics& stat ) { stat = statistics_; }

		bool operator == ( const interval& other ) const { return (upper() == other.upper()) && (lower() == other.lower()); }

		char get_mul_lookup_table_for( const interval& other ) const {
			// Initialize lookup table to all zeros
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

			return lookup;
		}
		
		static real_type zero() { return real_type(0); }
		
		static void set_round_down() { assert( !std::fesetround(FE_DOWNWARD) ); }
		
		static void set_round_up() { assert( !std::fesetround(FE_UPWARD) ); }
		
		static void record_indeterminate_result() { ++(statistics_.indeterminate_result_count); }
		static void record_arithmetic_op() { ++(statistics_.arithmetic_op_count); }

	private:
		real_type lower_;
		real_type upper_;
		static inline statistics statistics_ {0,0};
};

template<typename T>
interval<T> operator + ( const interval<T>& A, const interval<T>& B ) {
	// Store users current rounding mode
	int user_rounding_mode = std::fegetround();

	// Declare variables to store result
	T result_lower;
	T result_upper;

	// Perform calculations with appropriate rounding modes
	interval<T>::set_round_down();
	result_lower = A.lower() + B.lower();
	interval<T>::set_round_up();
	result_upper = A.upper() + B.upper();

	// Record arithmetic operation
	interval<T>::record_arithmetic_op();

	// Restore users rounding mode
	assert( !std::fesetround(user_rounding_mode) );
	
	return interval<T>( result_lower, result_upper );
}

template<typename T>
interval<T> operator - ( const interval<T>& A, const interval<T>& B ) {
	// Store users current rounding mode
	int user_rounding_mode = std::fegetround();

	// Declare variables to hold return value
	T result_lower;
	T result_upper;

	// Perform calculations with appropriate rounding modes
	interval<T>::set_round_down();
	result_lower = A.lower() - B.upper();
	interval<T>::set_round_up();
	result_upper = A.upper() - B.lower();

	// Record arithmetic operation
	interval<T>::record_arithmetic_op();

	// Restore users rounding mode
	assert( !std::fesetround(user_rounding_mode) );
	
	return interval<T>( result_lower, result_upper );
}

template<typename T>
interval<T> operator * ( const interval<T>& A, const interval<T>& B ) {
	// Store users current rounding mode
	int user_rounding_mode = std::fegetround();

	// Declare variables to hold return value
	T result_lower;
	T result_upper;

	// Create 8-bit lookup table for upcoming switch
	char lookup = A.get_mul_lookup_table_for( B );

	// Perform calculations with appropriate rounding modes
	switch( lookup ) {
		case (char)0x11:
			// both negative
			//std::cout << *this << '\n' << other << '\n' << "both negative" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.upper() * B.upper();
			interval<T>::set_round_up();
			result_upper = A.lower() * B.lower();
			break;
		case (char)0x21:
			// this negative other both
			//std::cout << *this << '\n' << other << '\n' << "this negative other both" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.lower() * B.upper();
			interval<T>::set_round_up();
			result_upper = A.lower() * B.lower();
			break;
		case (char)0x41:
			// this negative other positive
			//std::cout << *this << '\n' << other << '\n' << "this negative other positive" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.lower() * B.upper();
			interval<T>::set_round_up();
			result_upper = A.upper() * B.lower();
			break;
		case (char)0x12:
			// this both other negative
			//std::cout << *this << '\n' << other << '\n' << "this both other negative" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.upper() * B.lower();
			interval<T>::set_round_up();
			result_upper = A.lower() * B.lower();
			break;
		case (char)0x22:
			// both both
			//std::cout << *this << '\n' << other << '\n' << "both both" << "\n\n";
			interval<T>::set_round_down();
			result_lower = (A.lower() * B.upper() < A.upper() * B.lower()) ? (A.lower() * B.upper()) : (A.upper() * B.lower());
			interval<T>::set_round_up();
			result_upper = (A.lower() * B.lower() > A.upper() * B.upper()) ? (A.lower() * B.lower()) : (A.upper() * B.upper());
			break;
		case (char)0x42:
			// this both other positive
			//std::cout << *this << '\n' << other << '\n' << "this both other positive" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.lower() * B.upper();
			interval<T>::set_round_up();
			result_upper = A.upper() * B.upper();
			break;
		case (char)0x14:
			// this positive other negative
			//std::cout << *this << '\n' << other << '\n' << "this positive other negative" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.upper() * B.lower();
			interval<T>::set_round_up();
			result_upper = A.lower() * B.upper();
			break;
		case (char)0x24:
			// this positive other both
			//std::cout << *this << '\n' << other << '\n' << "this positive other both" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.upper() * B.lower();
			interval<T>::set_round_up();
			result_upper = A.upper() * B.upper();
			break;
		case (char)0x44:
			// this positive other positive
			//std::cout << *this << '\n' << other << '\n' << "both positive" << "\n\n";
			interval<T>::set_round_down();
			result_lower = A.lower() * B.lower();
			interval<T>::set_round_up();
			result_upper = A.upper() * B.upper();
			break;
		default:
			//std::cout << *this << '\n' << other << '\n' << "this or other zero" << "\n\n";
			result_lower = interval<T>::zero();
			result_upper = interval<T>::zero();
	}
			
	// Record arithmetic operation
	interval<T>::record_arithmetic_op();

	// Restore users rounding mode
	assert( !std::fesetround(user_rounding_mode) );
	
	return interval<T>( result_lower, result_upper );
}

template<typename T>
bool operator < ( const interval<T>& A, const interval<T>& B ) {
	if( (A.upper() < B.lower()) || (A.lower() >= B.upper()) ) {
		return A.upper() < B.lower();
	}else {
		interval<T>::record_indeterminate_result();
		throw indeterminate_result {"Cannot determine result of less-than comparison"};
	}
}

template<typename T>
std::ostream& operator << ( std::ostream& out, const interval<T>& insertee ) {
	out << '[';
	out << insertee.lower();
	out << ',';
	out << insertee.upper();
	out << ']';
	return out;
}
}
