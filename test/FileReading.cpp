#include <catch2/catch_test_macros.hpp>
#include "CliniParser.hpp"

using namespace std::literals;

TEST_CASE( "File reading" ) {
    const auto& res_str = get_file("test-file.ini");
    REQUIRE( res_str.is_valid() );
    const auto& vecres_rng = split_token(res_str.get(), fileline_re);
    REQUIRE( vecres_rng.is_valid() );
    const auto& vecres = vecres_rng.get();
    REQUIRE( to<std::string>(vecres[0]) == "truc=machin"s );
    REQUIRE( to<std::string>(vecres[1]) == "bidule=2"s );
    REQUIRE( to<std::string>(vecres[2]) == "blah=4,5,6"s );

    const auto& pairvec = split_keyvalue_pair(vecres[2]);
    REQUIRE( pairvec.is_valid() );
    REQUIRE( distance(begin(res_str.get()),begin(pairvec.get().second)) == 71 );
    REQUIRE( vector_parse<size_t>(pairvec.get().second).get() == std::vector<size_t>{4,5,6} );
}

TEST_CASE( "Command Line reading" ) {
    const std::string cmd_str{R"#(truc=machin bidule=2 blah=4,5,6)#"};
    const auto& vecres_rng = split_token(cmd_str, commandline_re);
    REQUIRE( vecres_rng.is_valid() );
    const auto& vecres = vecres_rng.get();
    REQUIRE( to<std::string>(vecres[0]) == "truc=machin"s );
    REQUIRE( to<std::string>(vecres[1]) == "bidule=2"s );
    REQUIRE( to<std::string>(vecres[2]) == "blah=4,5,6"s );

    const auto& pairvec = split_keyvalue_pair(vecres[2]);
    REQUIRE( pairvec.is_valid() );
    REQUIRE( distance(begin(cmd_str),begin(pairvec.get().second)) == 26 );
    REQUIRE( vector_parse<size_t>(pairvec.get().second).get() == std::vector<size_t>{4,5,6} );
}