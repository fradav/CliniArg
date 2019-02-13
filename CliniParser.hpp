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


template <class BidirIt>
using expected_keyvalue_pair = expected<std::pair<std::sub_match<BidirIt>,std::sub_match<BidirIt>>, // Payload
                 ParsingErrorsT>; // Eventual error


template <class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
const auto split_keyvalue_pair(Rng&& keyvalue_str)
{
    const auto& res = keyvalue_str | view::tokenize(keyvalue_re,{1,2});
    typedef typename range_value_type_t<decltype(res)>::iterator sub_match_it; 
    if (distance(res) == 2)
    {                     // successful parsing
        return expected_keyvalue_pair<sub_match_it>::success(*begin(res),*next(begin(res)));
    }
    else
    { // Failed parsing
        return expected_keyvalue_pair<sub_match_it>::error(ParsingErrorsT::keyvaluenotparsed);
    }
}


template <class BidirIt>
const auto split_keyvalue_pair(const std::sub_match<BidirIt>& t) {
    return split_keyvalue_pair(iterator_range(t.first,t.second));
}

template <class ValueT>
bool is_negative_integral(const std::string& value_str) {
    if constexpr (std::is_unsigned_v<ValueT>){
        std::make_signed_t<ValueT> n;
        std::stringstream ststr(value_str);
        return (ststr >> n) && n < 0;
    } else return false;
}   

/**
 * @brief Simple parsing function, using provided std::stringstream parser, should cover all base types
 * 
 * @tparam ValueT type to parse
 * @param value_str original string
 * @return auto 
 */
template <class ValueT, class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
const auto simple_parse(Rng&& value_str)
{
    const std::string& str_proxy{begin(value_str),end(value_str)}; 
    std::stringstream ststr(str_proxy);

    ValueT n;

    if (!is_negative_integral<ValueT>(str_proxy) // check for positive if unsigned type
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

template <class ValueT, class BidirIt>
const auto simple_parse(const std::sub_match<BidirIt>& t)
{
    return simple_parse<ValueT>(iterator_range(t.first,t.second));
}

/**
 * @brief template expected monad for vector
 * 
 * @tparam ValueT base type
 */
template <class ValueT>
using expected_vector = expected<std::vector<ValueT>, ParsingErrorsT>;

template <class BidirIt>
using sub_match_range_it = iterator_range<typename std::sub_match<BidirIt>::iterator>;

const std::regex vector_re{R"#([^,]+)#"};
/**
 * @brief Apply simple_parse on vector_parse
 * 
 * @tparam ValueT the expected type to parse
 * @param value_str 
 * @return expected_vector<ValueT>
 */
template <class ValueT, class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
const auto vector_parse(Rng&& value_str)
{
    // auto res = value_str | vector_split(',') | view::transform(simple_parse<ValueT>) | to_vector;
    const auto& res_token = value_str
        | view::tokenize(vector_re);
    typedef typename range_value_type_t<decltype(res_token)>::iterator sub_range_it; 
    const auto& res = res_token
        | view::transform(simple_parse<ValueT, sub_range_it>);
    if (distance(res) > 0)
    {
        bool parsed = accumulate(
                res | view::transform([](auto&& e) { return e.is_valid(); }), 
                true,
                std::logical_and());
        if (parsed)
        {
            const auto& vres = res 
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

template <class ValueT, class BidirIt>
const auto vector_parse(const std::sub_match<BidirIt>& t) {
    return vector_parse<ValueT>(iterator_range(t.first,t.second));
}

/**
 * @brief Various error related to file reading, kept in one enum
 * 
 */
enum FileAndArgsErrorsT
{
    filenotopened,
    fileioerror,
    argerror,
    empty
};

const std::regex line_re{R"#([^\r\n]+)#"};

const auto get_file(std::string filename)
{
    std::ifstream file_str(filename);
    if (file_str) {
        file_str >> std::noskipws;
        const std::string& fstr{std::istream_iterator<char>{file_str}, {}};
        if (file_str.bad()) 
            return expected<std::string,FileAndArgsErrorsT>::error(FileAndArgsErrorsT::fileioerror);
        else
            return expected<std::string,FileAndArgsErrorsT>::success(fstr);
    } else
        return expected<std::string,FileAndArgsErrorsT>::error(FileAndArgsErrorsT::filenotopened);
}

/**
 * @brief 
 * 
 * @param filename 
 * @return auto 
 */
template <class BidirIt>
using expected_args = expected<std::vector<std::sub_match<BidirIt>>,FileAndArgsErrorsT> ;

template<class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
const auto split_token(Rng&& str, const std::regex& re)
{
    const auto& res = str
                | view::tokenize(re);
    typedef typename range_value_type_t<decltype(res)>::iterator sub_match_it; 
    if (distance(res) > 0)
        return expected_args<sub_match_it>::success(res);  
    else
        return expected_args<sub_match_it>::error(FileAndArgsErrorsT::empty);
}
