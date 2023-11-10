#pragma once

#include "snir/ecs/handle.hpp"
#include "snir/ecs/storage.hpp"
#include "snir/ir/v2/inst_id.hpp"
#include "snir/ir/v2/type.hpp"
#include "snir/ir/v2/value_id.hpp"
#include "snir/ir/v2/value_kind.hpp"
#include "snir/ir/v2/value_registry.hpp"

#include <format>
#include <variant>
#include <vector>

namespace snir::v2 {

using Value = snir::Handle<ValueId>;

struct Literal
{
    std::variant<bool, std::int64_t, float, double> value;
};

struct Name
{
    std::string text;
};

struct BasicBlock
{
    ValueId label;
    std::vector<InstId> instructions;
};

struct FuncArguments
{
    std::vector<Type> args;
};

struct FuncBody
{
    // std::map<ValueId, BasicBlock> blocks;
    std::vector<BasicBlock> blocks;
};

}  // namespace snir::v2

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::Literal>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::Literal;
    using StorageType   = SparseStorage<snir::v2::Literal>;
};

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::Name>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::Name;
    using StorageType   = SparseStorage<snir::v2::Name>;
};

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::FuncArguments>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::FuncArguments;
    using StorageType   = SparseStorage<snir::v2::FuncArguments>;
};

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::FuncBody>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::FuncBody;
    using StorageType   = SparseStorage<snir::v2::FuncBody>;
};

template<>
struct std::formatter<snir::v2::Literal, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v2::Literal literal, FormatContext& fc) const
    {
        auto str = std::visit([](auto val) { return std::format("{}", val); }, literal.value);
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
