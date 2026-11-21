#pragma once
#include <stdexcept>
#include <string>

// Unlike assert(), CHECK stays enabled in Release builds too.
#define CHECK(condition)                                                     \
    do {                                                                     \
        if (!(condition))                                                    \
            throw std::runtime_error(std::string(__FILE__) + ":" +           \
                std::to_string(__LINE__) + " CHECK failed: " #condition);    \
    } while (false)
