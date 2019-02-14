#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../Singleton.hpp"
#include "../Settings.hpp"

using namespace std::literals;


TEST_CASE( "Singleton instance" ) {
    {
        auto& props = Singleton<Properties>::instance();
        props.oneint = 9;
        props.onevecfloat = std::vector<float>{3.5f,2.3f,4.0f};
        props.onestring = "trucmachin"s;
    }
    auto const& props = Singleton<Properties>::instance();
    REQUIRE( props.oneint == 9 );
    REQUIRE( props.onevecfloat == std::vector<float>{3.5f,2.3f,4.0f} );
    REQUIRE( props.onestring == "trucmachin"s );
    
}