#include "snir/core/ValueStore.hpp"

#include "fmt/os.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <utility>

#undef NDEBUG
#include <cassert>

namespace snir {

enum struct Type : std::uint8_t
{
    Void,
    Pointer,
    Bool,
    Integer,
    Float,
    Double,
};

enum struct ValueKind : std::uint8_t
{
    Register,
    Inst,
    Label,
    Function,
    Literal,
    Global,
};

enum struct ValueId : std::int32_t
{
};
enum struct RegisterId : std::int32_t
{
};
enum struct InstId : std::int32_t
{
};
enum struct LabelId : std::int32_t
{
};
enum struct FunctionId : std::int32_t
{
};
enum struct LiteralId : std::int32_t
{
};
enum struct GlobalId : std::int32_t
{
};

struct Value
{
    ValueKind kind;
    Type type;

    union
    {
        RegisterId registerId;
        InstId instructionId;
        LabelId labelId;
        FunctionId functionId;
        LiteralId literalId;
        GlobalId globalId;
    };
};

enum struct InstKind : std::int8_t
{
    Nop,
    Constant,

    Return,
    Branch,
    Phi,

    Add,
    Sub,
    Mul,
    Div,
    Rem,
    ShiftLeft,
    ShiftRight,
    And,
    Or,
    Xor,

    FloatAdd,
    FloatSub,
    FloatMul,
    FloatDiv,
    FloatRem,
};

enum struct UnaryOpId : std::int32_t
{
};
enum struct BinaryOpId : std::int32_t
{
};
enum struct BranchId : std::int32_t
{
};

struct Inst
{
    InstKind kind;
    ValueId result;

    union
    {
        UnaryOpId unaryOpId;
        BinaryOpId binaryOpId;
        BranchId branchId;
    };
};

struct UnaryOp
{
    enum Op : std::uint8_t
    {
        Const,
        Minus,
        Trunc,
    };

    Op op;
    ValueId operand;
};

struct BinaryOp
{
    enum Op : std::uint8_t
    {
        Add,
        Sub,
        Mul,
        Div,
    };

    Op op;
    ValueId lhs;
    ValueId rhs;
};

struct Branch
{
    ValueId condition;
    ValueId ifthen;
    ValueId ifelse;
};

struct SharedValueStore
{
    SharedValueStore()  = default;
    ~SharedValueStore() = default;

    SharedValueStore(SharedValueStore const& other)                    = delete;
    auto operator=(SharedValueStore const& other) -> SharedValueStore& = delete;

    SharedValueStore(SharedValueStore&& other)                    = default;
    auto operator=(SharedValueStore&& other) -> SharedValueStore& = default;

    [[nodiscard]] auto values() -> ValueStore<ValueId, Value>&;
    [[nodiscard]] auto values() const -> ValueStore<ValueId, Value> const&;

    [[nodiscard]] auto instructions() -> ValueStore<InstId, Inst>&;
    [[nodiscard]] auto instructions() const -> ValueStore<InstId, Inst> const&;

    [[nodiscard]] auto unaryOps() -> ValueStore<UnaryOpId, UnaryOp>&;
    [[nodiscard]] auto unaryOps() const -> ValueStore<UnaryOpId, UnaryOp> const&;

    [[nodiscard]] auto binaryOps() -> ValueStore<BinaryOpId, BinaryOp>&;
    [[nodiscard]] auto binaryOps() const -> ValueStore<BinaryOpId, BinaryOp> const&;

    [[nodiscard]] auto branches() -> ValueStore<BranchId, Branch>&;
    [[nodiscard]] auto branches() const -> ValueStore<BranchId, Branch> const&;

private:
    ValueStore<ValueId, Value> _values;
    ValueStore<InstId, Inst> _instructions;
    ValueStore<UnaryOpId, UnaryOp> _unaryOps;
    ValueStore<BinaryOpId, BinaryOp> _binaryOps;
    ValueStore<BranchId, Branch> _branchOps;
};

}  // namespace snir

namespace {

auto testValueStore() -> void
{
    enum struct Id : int
    {
    };

    auto store = snir::ValueStore<Id, int>{};
    store.reserve(2);
    assert(store.size() == 0);

    auto const id = store.addDefaultValue();
    assert(static_cast<int>(id) >= 0);
    assert(store.get(id) == 0);
    assert(store.size() == 1);

    auto& val = store.get(id);
    val       = 42;
    assert(std::as_const(store).get(id) == 42);

    auto const id2 = store.add(143);
    assert(store.get(id2) == 143);
    assert(store.size() == 2);
}

}  // namespace

auto main() -> int
{
    fmt::println("sizeof(Value): {}", sizeof(snir::Value));
    fmt::println("sizeof(Inst): {}", sizeof(snir::Inst));
    fmt::println("sizeof(UnaryOp): {}", sizeof(snir::UnaryOp));
    fmt::println("sizeof(BinaryOp): {}", sizeof(snir::BinaryOp));
    fmt::println("sizeof(Branch): {}", sizeof(snir::Branch));

    testValueStore();

    return EXIT_SUCCESS;
}
