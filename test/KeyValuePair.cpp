#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../CliniParser.hpp"
#include <range/v3/all.hpp>

using namespace std::literals;
// https://wandbox.org/permlink/hayy7C3HRCV3Wbqx

TEST_CASE( "Key/Value splitting") {
    std::string txt{"truc=machin"};
    const auto& res = split_keyvalue_pair(txt);
    REQUIRE( res.is_valid() );
    REQUIRE( std::string{res.get().first}  == "truc"s );
    REQUIRE( std::string{res.get().second}  == "machin"s );
    std::string txt2{"=trucmachin"};
    std::string txt3{"trucmachin="};
    REQUIRE( !split_keyvalue_pair(txt2).is_valid() );
    REQUIRE( !split_keyvalue_pair(txt3).is_valid() );

    std::string txt4{"truc=machin=bidule=35"};
    const auto& res2 = split_keyvalue_pair(txt4);
    REQUIRE( distance(begin(txt4),begin(res2.get().second))  == 5 );
    // const auto& res3 = split_keyvalue_pair(iterator_range(std::get<1>(res2.get())->first,std::get<1>(res2.get())->second));
    const auto& res3 = split_keyvalue_pair(res2.get().second);
    REQUIRE( distance(begin(txt4),begin(res3.get().second))  == 12 );
    const auto& res4 = split_keyvalue_pair(res3.get().second);
    REQUIRE( distance(begin(txt4),begin(res4.get().second))  == 19 );
}

