#include "snir/core/file.hpp"
#include "snir/core/print.hpp"
#include "snir/ir/v2/compare_kind.hpp"
#include "snir/ir/v2/inst_kind.hpp"
#include "snir/ir/v2/parser.hpp"
#include "snir/ir/v2/printer.hpp"
#include "snir/ir/v2/registry.hpp"
#include "snir/ir/v2/type.hpp"
#include "snir/ir/v2/value_kind.hpp"

#undef NDEBUG
#include <cassert>

namespace {

auto test() -> void
{
    using namespace snir::v2;

    auto registry = Registry{};

    auto func = registry.create(ValueKind::Function);
    func.emplace<Name>("func");
    func.emplace<Type>(Type::Void);
    func.patch<Type>([](auto& type) { type = Type::Double; });

    auto view         = registry.getValues().view<Name, Type>();
    auto [name, type] = view.get(func.getId());

    auto add = registry.create(InstKind::Add);
    add.emplace<Type>(Type::Float);

    auto br = registry.create(InstKind::IntCmp);
    br.emplace<Type>(Type::Int64);
    br.emplace<CompareKind>(CompareKind::Equal);
    br.emplace<Result>(ValueId{2});
    br.emplace<Operands>();

    snir::println("{} ({})", name.text.c_str(), type);
    snir::println("{} {}", int(add.get<InstKind>()), add.get<Type>());
}

auto testParser() -> void
{
    for (auto const& entry : std::filesystem::directory_iterator{"./test/files"}) {
        auto registry = snir::v2::Registry{};
        auto parser   = snir::v2::Parser{registry};
        auto error    = parser.read(snir::readFile(entry).value());
        assert(error.empty());

        snir::println("; {}", entry.path().string());
        auto printer = snir::v2::Printer{std::cout};
        printer(registry);
    }
}

}  // namespace

auto main() -> int
{
    test();
    testParser();
    return 0;
}
