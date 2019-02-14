#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../CliniParser.hpp"
#include <range/v3/all.hpp>

using namespace std::literals;

TEST_CASE( "File reading" ) {
    const auto& res_str = get_file("test-file.ini");
    REQUIRE( res_str.is_valid() );
    const auto& vecres_rng = split_token(res_str.get(), fileline_re);
    REQUIRE( vecres_rng.is_valid() );
    const auto& vecres = vecres_rng.get();
    REQUIRE( vecres[0].str() == "truc=machin"s );
    REQUIRE( vecres[1].str() == "bidule=2"s );
    REQUIRE( vecres[2].str() == "blah=4,5,6"s );

    const auto& pairvec = split_keyvalue_pair(vecres[2]);
    REQUIRE( pairvec.is_valid() );
    REQUIRE( distance(begin(res_str.get()),pairvec.get().second.first) == 26 );
    REQUIRE( vector_parse<size_t>(pairvec.get().second).get() == std::vector<size_t>{4,5,6} );
}

TEST_CASE( "Command Line reading" ) {
    const std::string cmd_str{R"#(truc=machin bidule=2 blah=4,5,6)#"};
    const auto& vecres_rng = split_token(cmd_str, commandline_re);
    REQUIRE( vecres_rng.is_valid() );
    const auto& vecres = vecres_rng.get();
    REQUIRE( vecres[0].str() == "truc=machin"s );
    REQUIRE( vecres[1].str() == "bidule=2"s );
    REQUIRE( vecres[2].str() == "blah=4,5,6"s );

    const auto& pairvec = split_keyvalue_pair(vecres[2]);
    REQUIRE( pairvec.is_valid() );
    REQUIRE( distance(begin(cmd_str),pairvec.get().second.first) == 26 );
    REQUIRE( vector_parse<size_t>(pairvec.get().second).get() == std::vector<size_t>{4,5,6} );
}