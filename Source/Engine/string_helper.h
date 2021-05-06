#pragma once

#include <string>

namespace string_helper
{
    // From type "std::string"  to  type "std::wstring"
    std::wstring stringToWString(const std::string_view str);
    // From type "std::wstring" to type "std::string"
    std::string  wStringToString(const std::wstring_view wstr);

    // Get the extension from the file path
    std::string  getExtension(const std::string_view filename);
    std::wstring getExtension(const std::wstring_view filename);

    // Remove the extension from the file path
    std::string  cutExtension(const std::string_view filename);
    std::wstring cutExtension(const std::wstring_view filename);

    // Get a directory path from a file path
    std::string  getDirectoryPath(const std::string_view filename);
    std::wstring getDirectoryPath(const std::wstring_view filename);

    // From type "std::string_view" to type "std::string"
    std::string  toString(const std::string_view str);
    std::wstring toWString(const std::wstring_view str);
} // namespace string_helper
