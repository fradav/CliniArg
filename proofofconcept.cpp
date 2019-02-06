#include <iostream>
#include <regex>
#include <sstream>
#include <iterator>
#include <type_traits>
#include <string_view>
#include <range/v3/all.hpp>

using namespace ranges;
using namespace std::literals;

const std::regex keyvalue_re(R"#(^([^=]+)=(.+)$)#");


template <typename... Args>
auto vector_tokenize(Args&&... args)
{
    return
        view::tokenize(std::forward<Args>(args)...)
        | view::transform([](auto&& rng){ return std::string_view(&*rng.first, rng.length()); })
        | to_vector;
}

template <typename... Args>
auto vector_split(Args&&... args)
{
    return
        view::split(std::forward<Args>(args)...)
        | view::transform([](auto&& rng){ return std::string_view(&*rng.begin(), distance(rng)); })
        | to_vector;
}

const auto vector_tokenize_re =
        vector_tokenize<const std::regex&,std::initializer_list<int>>;

template <typename Rng>
auto output_matches(Rng&& refstr)
{
    return [&refstr](auto&& t) { std::cout << t << " matched at : " << std::distance(begin(refstr), t.data()) << std::endl; };
}

using namespace ranges;

template <class T, class Rng>
T cxxparse(Rng&& rng) {
    std::stringstream s(std::forward<Rng>(rng));
    T v;
    s >> v;
    return v;
}

int main()
{
    const auto truc = "t ruc=machin=35"sv;
    auto res = truc | vector_tokenize_re(keyvalue_re,{1,2});
    for_each(res, output_matches(truc));
    const auto machin = "1,2,3,4"sv;
    for_each(machin | vector_split(','), output_matches(machin));
}