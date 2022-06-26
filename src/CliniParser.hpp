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
#pragma once
#include <regex>
#include <fstream>
#include <sstream>

#include "Expected.hpp"
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

typedef std::pair<size_t,ParsingErrorsT> ParsingErrorWithPositionT;

/**
 * @brief Regex for key value split
 * 
 */
const std::regex keyvalue_re(R"#(^([^=]+)=(.+)$)#");

template <class Rng>
using expected_keyvalue_pair = expected<std::pair<range_value_t<Rng>,range_value_t<Rng>>, // Payload
                 ParsingErrorsT>; // Eventual error

CPP_template(class Rng)
    (requires range<Rng>)
const auto split_keyvalue_pair(Rng&& keyvalue_str)
{
    const auto& res = keyvalue_str 
        | views::tokenize(keyvalue_re,{1,2})
        | views::move
        | views::transform([](auto&& t){
            return subrange(t.first,t.second);
        });

    if (distance(res) == 2)
    {                     // successful parsing
        return expected_keyvalue_pair<decltype(res)>::success(*begin(res),*next(begin(res)));
    }
    else
    { // Failed parsing
        return expected_keyvalue_pair<decltype(res)>::error(ParsingErrorsT::keyvaluenotparsed);
    }
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

CPP_template(class ValueT, class Rng)
    (requires range<Rng>)
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


/**
 * @brief template expected monad for vector
 * 
 * @tparam ValueT base type
 */
template <class ValueT>
using expected_vector = expected<std::vector<ValueT>, ParsingErrorsT>;

const std::regex vector_re{R"#([^,]+)#"};
/**
 * @brief Apply simple_parse on vector_parse
 * 
 * @tparam ValueT the expected type to parse
 * @param value_str 
 * @return expected_vector<ValueT>
 */
CPP_template(class ValueT, class Rng)
    (requires range<Rng>)
const auto vector_parse(Rng&& value_str)
{
    const auto& res_token = value_str
        | views::tokenize(vector_re)
        | views::transform([](auto&& t){
            return subrange(t.first,t.second);
        });
    typedef range_value_t<decltype(res_token)> sub_range_it; 
    const auto& res = res_token
        | views::transform(simple_parse<ValueT, sub_range_it>);
    if (distance(res) > 0)
    {
        bool parsed = accumulate(
                res | views::transform([](auto&& e) { return e.is_valid(); }), 
                true,
                std::logical_and());
        if (parsed)
        {
            const auto& vres = res 
                | views::transform([](auto&& e) { return e.get(); }) 
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
    argerror,
    empty
};

const std::regex fileline_re{R"/([^\r\n]+)/"};
const std::regex commandline_re{R"#(\S+)#"};

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
template <class Rng>
using expected_args = expected<std::vector<range_value_t<Rng>>,FileAndArgsErrorsT> ;

CPP_template(class Rng)
    (requires range<Rng>)
const auto split_token(Rng&& str, const std::regex& re)
{
    const auto& res = str
                | views::tokenize(re)
                | views::remove_if([](auto&& t){ return *(t.first) == '#' || *(t.first) == '%'; })
                | views::transform([](auto&& t){
                    return subrange(t.first,t.second);
                })
                | to_vector;
    if (distance(res) > 0)
        return expected_args<decltype(res)>::success(res);  
    else
        return expected_args<decltype(res)>::error(FileAndArgsErrorsT::empty);
}
