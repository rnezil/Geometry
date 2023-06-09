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

