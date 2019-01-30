/**
 * @file CliniParser.hpp
 * @author François-David Collin <Francois-David.Collin@umontpellier.fr>
 * @brief Basic parsing functions
 * @version 0.1
 * @date 2019-01-29
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <regex>
#include <sstream>
#include <iterator>
#include <type_traits>

#include "Expected.hpp"
#include "Settings.hpp"
#include <range/v3/all.hpp>

using namespace ranges;

/**
 * @brief Various error related to parsing, kept in one enum
 * 
 */
enum ParsingErrorsT
{
    keyvaluenotparsed,
    keynotfound,
    valuenotparsed,
    emptyvector,
    vectorvaluenotparsed
};

/**
 * @brief Regex for key value split
 * 
 */
const std::regex keyvalue_re(R"#(^([^=]+)=(.+)$)#");

/**
 * @brief expected type for key/value pair
 * 
 * Note : first type of expected template parameter is payload
 * second is the error.
 */
typedef expected<std::pair<std::string, std::string>, // Payload
                 ParsingErrorsT>                      // Eventual error
    expected_keyvalue_pair;

/**
 * @brief KeyValue split function 
 * 
 * @param keyvalue_str the key/value string
 * @return expected_keyvalue_pair
 */
auto split_keyvalue_pair(std::string keyvalue_str)
{
    std::smatch keyvalue_base_match;
    bool res = std::regex_search(keyvalue_str,        // original string
                                 keyvalue_base_match, // contains the matches
                                 keyvalue_re);        // regexp
    if (res)
    {                                                                   // successful parsing
        return expected_keyvalue_pair::success(keyvalue_base_match[1],  //key
                                               keyvalue_base_match[2]); // value
    }
    else
    { // Failed parsing
        return expected_keyvalue_pair::error(ParsingErrorsT::keyvaluenotparsed);
    }
}

/**
 * @brief Type testing for unsigned
 * 
 * @tparam T value
 */
template <class T>
using if_unsigned = std::enable_if_t<std::is_unsigned_v<T>>;

//// Beginning of SFINAE/enable_if Trick
// https://en.cppreference.com/w/cpp/language/sfinae
// Those overloads are for running a dedicated code to 
// detect the case of a negative parsed value in an unsigned type
/**
 * @brief Catch-all overload
 * 
 * @tparam ValueT 
 * @param ... 
 * @return false 
 */
template <class ValueT>
bool is_negative_integral(...) { return false; }

/**
 * @brief Overload for unsigned type
 * 
 * @tparam ValueT 
 * @tparam if_unsigned<ValueT> will be true only if Value t is unsigned
 * @param value_str the string to parse
 * @return true is the string parse to a signed value AND is negative
 * @return false  otherwise
 */
template <class ValueT, class = if_unsigned<ValueT>>
bool is_negative_integral(std::string value_str)
{
    typename std::make_signed<ValueT>::type n;
    std::stringstream ststr(value_str);
    return (ststr >> n) && n < 0;
}
//// End of SFINAE/enable_if Trick

/**
 * @brief Simple parsing function, using provided std::stringstream parser, should cover all base types
 * 
 * @tparam ValueT type to parse
 * @param value_str original string
 * @return auto 
 */
template <class ValueT>
auto simple_parse(std::string value_str)
{
    std::stringstream ststr(value_str);

    ValueT n;

    if (!is_negative_integral<ValueT>(value_str) // check for positive if unsigned type
        && ststr >> n  // parse and return true if parsed
        && ststr.eof()) // no residual (ex: parse 3.5 into 3)
    { // successful
        return expected<ValueT, ParsingErrorsT>::success(n);
    }
    else
    { // fail
        return expected<ValueT, ParsingErrorsT>::error(ParsingErrorsT::valuenotparsed);
    }
}

/**
 * @brief template expected monad for vector
 * 
 * @tparam ValueT base type
 */
template <class ValueT>
using expected_vector = expected<std::vector<ValueT>, ParsingErrorsT>;

/**
 * @brief Apply simple_parse on vector_parse
 * 
 * @tparam ValueT the expected type to parse
 * @param value_str 
 * @return expected_vector<ValueT>
 */
template <class ValueT>
auto vector_parse(std::string value_str)
{
    auto res = value_str | view::split(',') | view::transform(simple_parse<ValueT>) | to_vector;
    if (res.size() == 0)
    {
        return expected_vector<ValueT>::error(ParsingErrorsT::emptyvector);
    }
    else
    {
        bool parsed = accumulate(
            res | view::transform([](auto &&e) { return e.is_valid(); }), true,
            std::logical_and());
        if (parsed)
        {
            auto vres =
                res | view::transform([](auto &&e) { return e.get(); }) | to_vector;
            return expected_vector<ValueT>::success(vres);
        } else {
            return expected_vector<ValueT>::error(ParsingErrorsT::vectorvaluenotparsed);
        }

    }
}

// template <class SettingsT>
// auto parse_file_and_command_line(SettingsT&& s, std::string filename, const char **) {
//     auto filename_parsed = 
//         std:
// }