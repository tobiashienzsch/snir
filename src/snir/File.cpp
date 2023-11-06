#include "File.hpp"

namespace snir {

auto FileClose::operator()(std::FILE* file) -> void
{
    std::fclose(file);  // NOLINT(cppcoreguidelines-owning-memory)
}

auto openFile(std::filesystem::path const& path, char const* mode) -> File
{
#ifdef _WIN32
    auto wmode = std::array<wchar_t, 32>{};
    std::mbstowcs(wmode.data(), mode, std::strlen(mode));
    auto* file = ::_wfopen(path.c_str(), wmode.data());
#else
    auto* file = std::fopen(path.c_str(), mode);
#endif
    return File{file};
}

}  // namespace snir
