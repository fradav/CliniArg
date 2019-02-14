#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../CliniParser.hpp"
#include <range/v3/all.hpp>

using namespace std::literals;
// https://wandbox.org/permlink/hayy7C3HRCV3Wbqx

TEST_CASE( "Vector parsing") {
    REQUIRE( vector_parse<size_t>("1,2,3"s).is_valid() ); 
    REQUIRE( vector_parse<size_t>("1,2,3"s).get() == std::vector<size_t>{1,2,3} ); 
    REQUIRE( !vector_parse<size_t>("1,-2,3"s).is_valid() ); 
    REQUIRE( !vector_parse<size_t>("1,2,3.5"s).is_valid() ); 
    REQUIRE( vector_parse<double>("1,-2,3.5"s).is_valid() ); 
    REQUIRE( vector_parse<double>("1,-2,3.5"s).get() == std::vector<double>{1,-2,3.5} ); 
}

