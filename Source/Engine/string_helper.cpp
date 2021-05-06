#include "string_helper.h"

#include <algorithm>

#include <Windows.h>

namespace string_helper
{
    //-- From type "std::string"  to  type "std::wstring"  --//
    std::wstring stringToWString(const std::string_view str)
    {
        std::wstring wide_string{ str.begin(), str.end() };
        return wide_string;
    }
    //-- From type "std::wstring" to type "std::string" --//
    std::string  wStringToString(const std::wstring_view wstr)
    {
        const int length = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, nullptr, 0, nullptr, nullptr);

        std::string str{};
        str.resize(length);

        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, const_cast<char*>(str.data()), length, nullptr, nullptr);

        return str;
    }

    //-- Get the extension from the file path --//
    std::string  getExtension(const std::string_view filename)
    {
        const auto extension = filename.substr(filename.rfind('.') + 1);
        return toString(extension);
    }
    std::wstring getExtension(const std::wstring_view filename)
    {
        const auto extension = filename.substr(filename.rfind(L'.') + 1);
        return toWString(extension);
    }

    //-- Remove the extension from the file path --//
    std::string  cutExtension(const std::string_view filename)
    {
        const auto without_ext = filename.substr(0, filename.rfind('.'));
        return toString(without_ext);
    }
    std::wstring cutExtension(const std::wstring_view filename)
    {
        const auto without_ext = filename.substr(0, filename.rfind(L'.'));
        return toWString(without_ext);
    }

    //-- Get a directory path from a file path --//
    std::string  getDirectoryPath(const std::string_view filename)
    {
        auto index = filename.rfind('\\');
        if (index == std::string::npos) index = 0;
        index = std::max(index, filename.rfind('/'));
        if (index == std::string::npos) return {};

        const auto dir_path = filename.substr(0, index + 1);
        return std::string{ dir_path.begin(), dir_path.end() };
    }
    std::wstring getDirectoryPath(const std::wstring_view filename)
    {
        auto index = filename.rfind(L'\\');
        if (index == std::wstring::npos) index = 0;
        index = (std::max)(index, filename.rfind(L'/'));
        if (index == std::wstring::npos) return {};

        const auto dir_path = filename.substr(0, index + 1);
        return std::wstring{ dir_path.begin(), dir_path.end() };
    }

    //-- From type "std::string_view" to type "std::string" --//
    std::string  toString(const std::string_view str)
    {
        return std::string{ str.begin(), str.end() };
    }
    std::wstring toWString(const std::wstring_view str)
    {
        return std::wstring{ str.begin(), str.end() };
    }
} // namespace string_helper
