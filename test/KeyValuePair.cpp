#include <catch2/catch_test_macros.hpp>
#include "CliniParser.hpp"

using namespace std::literals;
// https://wandbox.org/permlink/hayy7C3HRCV3Wbqx

TEST_CASE( "Key/Value splitting") {
    std::string txt{"truc=machin"};
    const auto& res = split_keyvalue_pair(txt);
    REQUIRE( res.is_valid() );
    REQUIRE( to<std::string>(res.get().first) == "truc"s );
    REQUIRE( to<std::string>(res.get().second) == "machin"s );
    std::string txt2{"=trucmachin"};
    std::string txt3{"trucmachin="};
    REQUIRE( !split_keyvalue_pair(txt2).is_valid() );
    REQUIRE( !split_keyvalue_pair(txt3).is_valid() );

    std::string txt4{"truc=machin=bidule=35"};
    const auto& res2 = split_keyvalue_pair(txt4);
    REQUIRE( distance(begin(txt4),begin(res2.get().second))  == 5 );
    const auto& res3 = split_keyvalue_pair(res2.get().second);
    REQUIRE( distance(begin(txt4),begin(res3.get().second))  == 12 );
    const auto& res4 = split_keyvalue_pair(res3.get().second);
    REQUIRE( distance(begin(txt4),begin(res4.get().second))  == 19 );
}
