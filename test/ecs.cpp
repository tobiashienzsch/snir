#include "snir/core/print.hpp"
#include "snir/ir/v2/compare_kind.hpp"
#include "snir/ir/v2/inst_kind.hpp"
#include "snir/ir/v2/module_registry.hpp"
#include "snir/ir/v2/type.hpp"
#include "snir/ir/v2/value_kind.hpp"

#undef NDEBUG
#include <cassert>

namespace {

auto test() -> void
{
    using namespace snir::v2;

    auto reg = ModuleRegistry{};

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

}  // namespace

auto main() -> int
{
    test();
    return 0;
}
