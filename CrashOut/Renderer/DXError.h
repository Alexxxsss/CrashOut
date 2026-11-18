#pragma once

#include <windows.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>

namespace DX
{
    inline std::string FormatHRESULT(HRESULT hr)
    {
        std::ostringstream stream;

        stream << "0x"
            << std::uppercase
            << std::hex
            << std::setw(8)
            << std::setfill('0')
            << static_cast<uint32_t>(hr);

        return stream.str();
    }

    inline void Check(
        HRESULT hr,
        const std::string& message = "DirectX call failed")
    {
        if (FAILED(hr))
        {
            throw std::runtime_error(
                message +
                "\nHRESULT: " +
                FormatHRESULT(hr)
            );
        }
    }
}
