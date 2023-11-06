#pragma once

#include <cstdio>
#include <filesystem>
#include <memory>

namespace snir {

struct FileClose
{
    auto operator()(std::FILE* file) -> void;
};

using File = std::unique_ptr<std::FILE, FileClose>;

[[nodiscard]] auto openFile(std::filesystem::path const& path, char const* mode) -> File;

}  // namespace snir
