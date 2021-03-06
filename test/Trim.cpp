#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../trimandlower.hpp"


TEST_CASE( "Trim and lowercase") {
    REQUIRE( trim_spaces_underscores("Text\n __with\tsome \t  whitespaces and_ _underscores\n\n") == "Textwithsomewhitespacesandunderscores");
    REQUIRE( trim_spaces_underscores_andlower("Text\n __with\tsome \t  whitespaces and_ _underscores\n\n") == "textwithsomewhitespacesandunderscores");
}

