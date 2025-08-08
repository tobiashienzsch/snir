#pragma once

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace snir {

[[nodiscard]] inline auto readFile(std::filesystem::path const& path) -> std::optional<std::string>
{
    auto file = std::ifstream(path, std::ios::binary | std::ios::in);
    if (not file.is_open()) {
        return std::nullopt;
    }

    using Iterator = std::istreambuf_iterator<char>;
    auto content   = std::string(Iterator{file}, Iterator{});
    if (not file) {
        return std::nullopt;
    }

    auto replaceWindowsWithUnix = [](std::string str) {
        auto pos = std::size_t(0);
        while ((pos = str.find("\r\n", pos)) != std::string::npos) {
            str.replace(pos, 2, "\n");
            ++pos;
        }
        return str;
    };

    return replaceWindowsWithUnix(std::move(content));
}

}  // namespace snir
