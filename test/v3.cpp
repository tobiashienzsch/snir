#include "snir/ir/v3/CompareKind.hpp"
#include "snir/ir/v3/InstKind.hpp"
#include "snir/ir/v3/Parser.hpp"
#include "snir/ir/v3/Printer.hpp"
#include "snir/ir/v3/Registry.hpp"
#include "snir/ir/v3/Type.hpp"

#include "snir/core/file.hpp"
#include "snir/core/print.hpp"

#undef NDEBUG
#include <cassert>
#include <filesystem>

namespace ir = snir::v3;

auto main() -> int
{
    for (auto const& entry : std::filesystem::directory_iterator{"./test/files"}) {
        auto registry = snir::v3::Registry{};
        auto parser   = snir::v3::Parser{registry};
        auto source   = snir::readFile(entry).value();
        auto module   = parser.read(source);
        assert(module.has_value());

        snir::println("; {}", entry.path().string());
        auto printer = snir::v3::Printer{std::cout};
        printer(*module);
    }

    return 0;
}
