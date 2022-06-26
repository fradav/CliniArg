#include <catch2/catch_test_macros.hpp>
#include "CliniParser.hpp"

using namespace std::literals;

TEST_CASE( "Various single value parsing") {
    auto res = simple_parse<size_t>("50"s);
    REQUIRE( res.is_valid() );
    REQUIRE( res.get() == 50 );
    REQUIRE( !simple_parse<size_t>("-50"s).is_valid() );
    REQUIRE( simple_parse<int>("-50"s).get() == -50 );
    REQUIRE( simple_parse<double>("-2.3"s).get() == -2.3 );

}
