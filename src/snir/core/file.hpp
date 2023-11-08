#pragma once

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

    return content;
}

}  // namespace snir
