#pragma once

#include <string>
#include <algorithm>
#include <locale>
#include <range/v3/all.hpp>

using namespace ranges;

namespace detail
{
    /**
     * @brief Return true if space or "_"
     *
     * @param c
     * @return true
     * @return false
     */
    bool to_trim(char c)
    {
        return std::isspace(c, std::locale()) || (c == '_');
    }
} // namespace detail

/**
 * @brief removes spaces and underscores from a string
 * 
 * @param s 
 * @return std::string 
 */
std::string trim_spaces_underscores(std::string s)
{
    s |= actions::remove_if(::detail::to_trim);
    return s;
}

/**
 * @brief lower all char in a string
 * 
 * @param s 
 * @return std::string 
 */
std::string str_tolower(std::string s)
{
    // https://en.cppreference.com/w/cpp/string/byte/tolower
    s |= actions::transform([](auto c)
                            { return std::tolower(c, std::locale()); });
    return s;
}

/**
 * @brief compose two previous functions
 * 
 * @param s 
 * @return std::string 
 */
std::string trim_spaces_underscores_andlower(std::string s)
{
    return str_tolower(trim_spaces_underscores(s));
}