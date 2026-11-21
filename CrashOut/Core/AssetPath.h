#pragma once

#include <windows.h>
#include <array>
#include <filesystem>
#include <stdexcept>

namespace CrashOut
{
    inline std::filesystem::path AssetPath(
        const std::filesystem::path& relativePath)
    {
        std::array<wchar_t, 32768> buffer{};

        DWORD length = GetModuleFileNameW(
            nullptr,
            buffer.data(),
            static_cast<DWORD>(buffer.size())
        );

        if (length == 0 || length >= buffer.size())
        {
            throw std::runtime_error(
                "Cannot locate executable directory"
            );
        }

        std::filesystem::path exePath(buffer.data());

        return (exePath.parent_path() / relativePath)
            .lexically_normal();
    }
}
