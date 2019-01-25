#pragma once
#include <regex>
#include "Expected.hpp"
#include "Settings.hpp"

const std::regex keyvalue_re(R"#(^([^=]+)=(.+)$)")


