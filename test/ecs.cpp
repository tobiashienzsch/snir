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

    auto reg = Registry{};

    auto func = reg.create(ValueKind::Function);
    func.emplace<Name>("func");
    func.emplace<Type>(Type::Void);
    func.patch<Type>([](auto& type) { type = Type::Double; });

    auto view         = reg.getValueRegistry().view<Name, Type>();
    auto [name, type] = view.get(func.getId());

    auto add = reg.create(InstKind::Add);
    add.emplace<Type>(Type::Float);

    auto br = reg.create(InstKind::IntCmp);
    br.emplace<Type>(Type::Int64);
    br.emplace<CompareKind>(CompareKind::Equal);
    br.emplace<Result>(ValueId{2});
    br.emplace<Operands>();

    snir::println("{} ({})", name.text.c_str(), type);
    snir::println("{} {}", int(add.get<InstKind>()), add.get<Type>());
}

auto test_parser() -> void
{
    auto reg    = snir::v2::Registry{};
    auto parser = snir::v2::Parser{reg};
    auto module = parser.readModule(snir::readFile("./test/files/funcs.ll").value_or(""));
    assert(module.has_value());
    assert(module->getFunctions().size() == 3);

    auto printer = snir::v2::Printer{std::cout};
    printer(*module);
}

}  // namespace

auto main() -> int
{
    test();
    test_parser();
    return 0;
}
