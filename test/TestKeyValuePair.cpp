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
    REQUIRE( (res.get().first)->str()  == "truc"s );
    REQUIRE( (res.get().second)->str()  == "machin"s );
    std::string txt2{"=trucmachin"};
    std::string txt3{"trucmachin="};
    REQUIRE( !split_keyvalue_pair(txt2).is_valid() );
    REQUIRE( !split_keyvalue_pair(txt3).is_valid() );

    std::string txt4{"truc=machin=bidule=35"};
    const auto& res2 = split_keyvalue_pair(txt4);
    REQUIRE( distance(begin(txt4),(res2.get().second)->first)  == 5 );
    // const auto& res3 = split_keyvalue_pair(iterator_range(std::get<1>(res2.get())->first,std::get<1>(res2.get())->second));
    const auto& res3 = split_keyvalue_pair(res2.get().second);
    REQUIRE( distance(begin(txt4),(res3.get().second)->first)  == 12 );
    const auto& res4 = split_keyvalue_pair(res3.get().second);
    REQUIRE( distance(begin(txt4),(res4.get().second)->first)  == 19 );
}

TEST_CASE( "Various single value parsing") {
    auto res = simple_parse<size_t>("50"s);
    REQUIRE( res.is_valid() );
    REQUIRE( res.get() == 50 );
    REQUIRE( !simple_parse<size_t>("-50"s).is_valid() );
    REQUIRE( simple_parse<int>("-50"s).get() == -50 );
    REQUIRE( simple_parse<double>("-2.3"s).get() == -2.3 );

}

TEST_CASE( "Vector parsing") {
    REQUIRE( vector_parse<size_t>("1,2,3"s).is_valid() ); 
    REQUIRE( vector_parse<size_t>("1,2,3"s).get() == std::vector<size_t>{1,2,3} ); 
    REQUIRE( !vector_parse<size_t>("1,-2,3"s).is_valid() ); 
    REQUIRE( !vector_parse<size_t>("1,2,3.5"s).is_valid() ); 
    REQUIRE( vector_parse<double>("1,-2,3.5"s).is_valid() ); 
    REQUIRE( vector_parse<double>("1,-2,3.5"s).get() == std::vector<double>{1,-2,3.5} ); 
}

TEST_CASE( "file reading" ) {
    const auto& res = get_and_split_file("test-file.ini");
    REQUIRE( res.is_valid() );
    const auto& vecres = res.get() | to_vector;
    REQUIRE( vecres[0].str() == "truc=machin"s );
    REQUIRE( vecres[1].str() == "bidule=2"s );
    REQUIRE( vecres[2].str() == "blah=4,5,6"s );

}