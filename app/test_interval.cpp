#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "ra/interval.hpp"
#include <iostream>

TEMPLATE_TEST_CASE("Check default constructor with parameter", "[constructor]", float, double, long double) {
	ra::math::interval zerod {0};
	ra::math::interval zerod_expl {0};
	ra::math::interval pos {26.27};
	ra::math::interval neg {-420.69};
	CHECK( zerod == zerod_expl );
	CHECK( pos.upper() == 26.27 );
	CHECK( pos.lower() == 26.27 );
	CHECK( neg.upper() == -420.69 );
	CHECK( neg.lower() == -420.69 );
}

TEST_CASE("Check default constructor without parameter", "[constructor]") {
	ra::math::interval<float> zerof;
	ra::math::interval<double> zerod;
	ra::math::interval<long double> zerold;
	ra::math::interval<float> zerof_e {0};
	ra::math::interval<double> zerod_e {0};
	ra::math::interval<long double> zerold_e {0};
	CHECK( zerof == zerof_e );
	CHECK( zerod == zerod_e );
	CHECK( zerold == zerold_e );
}

TEMPLATE_TEST_CASE("Check *= operator", "[operator]", float, double, long double) {
	ra::math::interval a {-2,-1};
	ra::math::interval b {-1, 1};
	ra::math::interval c {1, 2};
	ra::math::interval aa {-2,-1};
	ra::math::interval bb {-1, 1};
	ra::math::interval cc {1, 2};
	ra::math::interval aaa {-2,-1};
	ra::math::interval bbb {-1, 1};
	ra::math::interval ccc {1, 2};
	ra::math::interval aaaa {-2,-1};
	ra::math::interval bbbb {-1, 1};
	ra::math::interval aaaaa {-2,-1};
	ra::math::interval nn {1, 4};
	ra::math::interval nb {-2, 2};
	ra::math::interval np {-4, -1};
	ra::math::interval bn {-2, 2};
	ra::math::interval bobo {-1, 1};
	ra::math::interval bp {-2, 2};
	ra::math::interval pn {-4, -1};
	ra::math::interval pb {-2, 2};
	ra::math::interval pp {1, 4};
	ra::math::interval xerox {0,0};

	a *= a;//both neg
	aa *= b;//this neg other both
	aaa *= c;//this neg other pos
	b *= aaaa;//this both other neg
	bb *= bb;//both both
	bbb *= c;//this both other pos
	c *= aaaaa;//this pos other neg
	cc *= bbbb;//this pos other neg
	ccc *= ccc;//both pos
	CHECK( a == nn );
	CHECK( aa == nb );
	CHECK( aaa == np );
	CHECK( b == bn );
	CHECK( bb == bobo );
	CHECK( bbb == bp );
	CHECK( c == pn );
	CHECK( cc == pb );
	CHECK( ccc == pp );
	nn *= xerox;
	nb *= xerox;
	np *= xerox;
	bn *= xerox;
	bobo *= xerox;
	bp *= xerox;
	pn *= xerox;
	pb *= xerox;
	pp *= xerox;
	xerox *= c;
	xerox *= bb;
	xerox *= a;
	CHECK( nn == xerox );
	CHECK( nb == xerox );
	CHECK( np == xerox );
	CHECK( bn == xerox );
	CHECK( bobo == xerox );
	CHECK( bp == xerox );
	CHECK( pn == xerox );
	CHECK( pb == xerox );
	CHECK( pp == xerox );
}

TEMPLATE_TEST_CASE("Check * operator", "[operator]", float, double, long double) {
	ra::math::interval a {-2,-1};
	ra::math::interval b {-1, 1};
	ra::math::interval c {1, 2};
	
	ra::math::interval a2 {-2,-1};
	ra::math::interval b2 {-1, 1};
	ra::math::interval c2 {1, 2};
	ra::math::interval aa2 {-2,-1};
	ra::math::interval bb2 {-1, 1};
	ra::math::interval cc2 {1, 2};
	ra::math::interval aaa2 {-2,-1};
	ra::math::interval bbb2 {-1, 1};
	ra::math::interval ccc2 {1, 2};
	ra::math::interval aaaa2 {-2,-1};
	ra::math::interval bbbb2 {-1, 1};
	ra::math::interval aaaaa2 {-2,-1};

	CHECK( a * a == (a2 *= a2) );
	CHECK( a * b == (aa2 *= b2) );
	CHECK( a * c == (aaa2 *= c2) );
	CHECK( b * a == (b2 *= aaaa2) );
	CHECK( b * b == (bb2 *= bb2) );
	CHECK( b * c == (bbb2 *= c2) );
	CHECK( c * a == (c2 *= aaaaa2) );
	CHECK( c * b == (cc2 *= bbbb2) );
	CHECK( c * c == (ccc2 *= ccc2) );
}

TEMPLATE_TEST_CASE(" Check +, +=, -, and -= operators", "[operator]", float, double, long double) {
	ra::math::interval a {-32.67, 69.42};
	ra::math::interval b {59.59, 100.3};
	ra::math::interval c {-10000.5, 1.1218};

	ra::math::interval a2 {-32.67, 69.42};
	ra::math::interval b2 {59.59, 100.3};
	ra::math::interval c2 {-10000.5, 1.1218};

	ra::math::interval a3 {-32.67, 69.42};
	ra::math::interval b3 {59.59, 100.3};
	ra::math::interval c3 {-10000.5, 1.1218};

	CHECK( a + b == (a2 += b2) );
	CHECK( b + c == (b2 += c2) );
	CHECK( c + c == (c2 += c2) );
	CHECK( c - a == (c3 -= a3) );
	CHECK( a - b == (a3 -= b3) );
	CHECK( b - b == (b3 -= b3) );
}

TEMPLATE_TEST_CASE("Check is_singleton and sign", "[function]", float, double, long double) {
	ra::math::interval a {-4.20};
	ra::math::interval b {6.9};
	ra::math::interval c {0.0};
	ra::math::interval d {-2.2, 3.3};
	ra::math::interval e {-49.999, -48.888};
	ra::math::interval f {50.1, 100.2};
	ra::math::interval g {0.0, 0.000};

	CHECK( a.is_singleton() == true );
	CHECK( b.is_singleton() == true );
	CHECK( c.is_singleton() == true );
	CHECK( d.is_singleton() == false);
	CHECK( e.is_singleton() == false);
	CHECK( f.is_singleton() == false);
	CHECK( a.sign() == -1 );
	CHECK( b.sign() == 1 );
	CHECK( c.sign() == 0 );
	try{
		int exc = d.sign();
	}catch( ra::math::indeterminate_result& e ){
		std::cout << "Caught an exception.\n";
	}
	CHECK( e.sign() == -1 );
	CHECK( f.sign() == 1 );
	CHECK( g.sign() == 0 );
	CHECK( g.is_singleton() == true );
}

TEMPLATE_TEST_CASE("Test < operator", "[operator]", float, double, long double) {
	ra::math::interval a {-90.3, -80.4};
	ra::math::interval b {-80.4, 0.0};
	ra::math::interval c {-85.1, -77.7};
	ra::math::interval d {-120.2, -112.1};

	CHECK( d < a );
	CHECK( !(b < a) );
	try{
		bool rachel = a < c;
	}catch( ra::math::indeterminate_result& e ) {
		std::cout << "Exception from < operation caught.\t" << e.what() << '\n';
	}
}
