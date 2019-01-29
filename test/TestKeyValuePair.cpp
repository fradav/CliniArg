#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../CliniParser.hpp"


TEST_CASE( "Key/Value splitting") {
    auto res = split_keyvalue_pair("truc=machin");
    REQUIRE( res.is_valid() );
    REQUIRE( res.get() == std::make_pair<std::string,std::string>("truc","machin") );
    REQUIRE( !split_keyvalue_pair("=trucmachin").is_valid() );
    REQUIRE( !split_keyvalue_pair("trucmachin=").is_valid() );
}

TEST_CASE( "Various single value parsing") {
    auto res = simple_parse<size_t>("50");
    REQUIRE( res.is_valid() );
    REQUIRE( res.get() == 50 );
    REQUIRE( !simple_parse<size_t>("-50").is_valid() );
    REQUIRE( simple_parse<int>("-50").get() == -50 );
    REQUIRE( simple_parse<double>("-2.3").get() == -2.3 );
}

TEST_CASE( "Vector parsing") {
    REQUIRE( vector_parse<size_t>(std::string("1,2,3")).is_valid() ); 
    REQUIRE( !vector_parse<size_t>(std::string("1,-2,3")).is_valid() ); 
    REQUIRE( !vector_parse<size_t>(std::string("1,2,3.5")).is_valid() ); 
    REQUIRE( vector_parse<double>(std::string("1,-2,3.5")).is_valid() ); 
}

