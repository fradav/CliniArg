#pragma once
#include <vector>
#include <map>
#include "Expected.hpp"


enum ParamT {
    oneint,
    onevectflot,
    onestring
};

enum ParsingErrorsT {
    keynotparsed,
    valuenotparsed
};

std::map<std::string,ParamT> MapStringToParam {
    { "oneint", ParamT::oneint },
    { "onevectflot", ParamT::onevectflot },
    { "onestring", ParamT::onestring }
};

struct Properties {
    size_t oneint;
    std::vector<float> onevectflot;
    std::string onestring;
}

template<class ValueT>
expected<ParsingErrorsT,ValueT> parse_onekeyvalue_pair(std::string key,std::string value) {
    
}

