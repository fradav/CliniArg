/**
 * @file Settings.hpp
 * @author François-David Collin <Francois-David.Collin@umontpellier.fr>
 * @brief Global settings
 * @version 0.1
 * @date 2019-01-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <vector>
#include <map>

/**
 * @brief Parameters enum
 * 
 */
enum ParamT {
    oneint,
    onevectflot,
    onestring
};

/**
 * @brief Map from parameter string to param enum
 * 
 */
std::map<std::string,ParamT> MapStringToParam {
    { "oneint", ParamT::oneint },
    { "onevectflot", ParamT::onevectflot },
    { "onestring", ParamT::onestring }
};

/**
 * @brief Properties contains the actual parameters
 * 
 * 
 */
struct Properties {
    size_t oneint;
    std::vector<float> onevectflot;
    std::string onestring;
};


