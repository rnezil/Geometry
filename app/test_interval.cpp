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
