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
#include <string_view>
#include <functional>
#include <ios>

#include "Expected.hpp"
#include "Settings.hpp"
#include <range/v3/all.hpp>

using namespace ranges;


// template <typename t>
// using range_reference_t = decltype(*std::begin(std::declval<t &>()));

// template <typename t>
// using iterator_t = decltype(std::begin(std::declval<t &>()));

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

typedef std::pair<size_t,ParsingErrorsT> ParsingErrorWithPositionT;

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
template <class Rng>
using expected_keyvalue_pair = 
    expected<std::pair<Rng,Rng>, // Payload
                ParsingErrorsT>; // Eventual error


template <class Rng>
using is_lvalueref_noarray = 
    std::enable_if_t<
        std::is_lvalue_reference_v<Rng> && 
        !std::is_array_v<std::remove_reference_t<Rng>>>;

/**
 * @brief KeyValue split function 
 * 
 * @param keyvalue_str the key/value string
 * @return expected_keyvalue_pair
 */
// template <class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
// auto split_keyvalue_pair(Rng&& keyvalue_str)
// template <class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
template <class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
auto split_keyvalue_pair(Rng&& keyvalue_str)
{
    const auto& res = keyvalue_str | view::tokenize(keyvalue_re,{1,2});
    typedef iterator_t<decltype(res)> ref_t;
    if (distance(res) == 2)
    {                     // successful parsing
        return expected_keyvalue_pair<ref_t>::success(begin(res),next(begin(res),1));
    }
    else
    { // Failed parsing
        return expected_keyvalue_pair<ref_t>::error(ParsingErrorsT::keyvaluenotparsed);
    }
}
template <class Bidir>
auto split_keyvalue_pair(const std::regex_token_iterator<Bidir> t) {
    return split_keyvalue_pair(iterator_range(t->first,t->second));
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
// template <class ValueT, class Rng,  class = if_unsigned<ValueT>>
// bool is_negative_integral(Rng&& value_str)
template <class ValueT,  class Rng, CONCEPT_REQUIRES_(Range<Rng>()), class = if_unsigned<ValueT>>
bool is_negative_integral(Rng&& value_str)
{
    typename std::make_signed<ValueT>::type n;
    std::stringstream ststr(std::string(value_str.begin(),value_str.end()));
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
// template <class ValueT, class Rng, CONCEPT_REQUIRES_(Range<Rng>()) >
// auto simple_parse(Rng&& value_str)
template <class ValueT, class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
auto simple_parse(Rng&& value_str)
{
    std::stringstream ststr(std::string(value_str.begin(),value_str.end()));

    ValueT n;

    if (!is_negative_integral<ValueT>(std::forward<Rng>(value_str)) // check for positive if unsigned type
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
template <class ValueT, class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
auto vector_parse(Rng&& value_str)
{
    // auto res = value_str | vector_split(',') | view::transform(simple_parse<ValueT>) | to_vector;
    auto res = value_str 
        | view::split(',') 
        | view::transform(simple_parse<ValueT,Rng>) ;
    if (distance(res) > 0)
    {
        bool parsed = accumulate(
                res | view::transform([](auto&& e) { return e.is_valid(); }), 
                true,
                std::logical_and());
        if (parsed)
        {
            auto vres = res 
                | view::transform([](auto&& e) { return e.get(); }) 
                | to_vector;
            return expected_vector<ValueT>::success(std::move(vres));
        } else {
            return expected_vector<ValueT>::error(ParsingErrorsT::vectorvaluenotparsed);
        }
    }
    else
    {
       return expected_vector<ValueT>::error(ParsingErrorsT::emptyvector); 
    }
}


/**
 * @brief Various error related to file reading, kept in one enum
 * 
 */
enum FileAndArgsErrorsT
{
    filenotopened,
    fileioerror,
    argerror
};

const std::regex line_re{R"#([^\r\n]+)#"};
const std::string& _empty{""};
typedef typename iterator_t<decltype(_empty | view::tokenize(std::regex()))>::value_type sub_match_char;
typedef expected<std::pairdecltype(_empty | view::tokenize(std::regex())),FileAndArgsErrorsT> expected_args;

/**
 * @brief 
 * 
 * @param filename 
 * @return auto 
 */
auto get_and_split_file(std::string filename)
{
    std::ifstream file_str(filename);
    if (file_str) {
        file_str >> std::noskipws;
        const std::string& fstr{std::istream_iterator<char>{file_str}, {}};
        if (file_str.bad()) 
            return expected_args::error(FileAndArgsErrorsT::fileioerror);
        else {
            const auto& res = fstr
                | view::tokenize(line_re)
                | view::move;
            return expected_args::success(res);      
        }
    } else 
        return expected_args::error(FileAndArgsErrorsT::filenotopened);
}
