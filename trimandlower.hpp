#pragma once

#include <string>
#include <algorithm>
#include <locale>
#include <cctype>
#include <range/v3/all.hpp>

using namespace ranges;

namespace detail
{
// Return true if space or "_"
bool to_trim(unsigned char c)
{
    return std::isspace(c, std::locale()) || (c == '_');
}
} // namespace detail

std::string trim_spaces_underscores(std::string s)
{
    s |= action::remove_if(::detail::to_trim);
    return s;
}

std::string str_tolower(std::string s)
{
    // https://en.cppreference.com/w/cpp/string/byte/tolower
    s |= action::transform([](char c){ return static_cast<char>(std::tolower(c, std::locale())); }); 
    return s;
}

std::string trim_spaces_underscores_andlower(std::string s)
{
    return str_tolower(trim_spaces_underscores(s));
}