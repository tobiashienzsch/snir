#pragma once

#include "snir/ecs/handle.hpp"
#include "snir/ecs/storage.hpp"
#include "snir/ir/v2/inst_id.hpp"
#include "snir/ir/v2/type.hpp"
#include "snir/ir/v2/value_id.hpp"
#include "snir/ir/v2/value_kind.hpp"
#include "snir/ir/v2/value_registry.hpp"

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
    std::vector<InstId> instructions;
};

struct FunctionArguments
{
    std::vector<Type> args;
};

struct FunctionBody
{
    std::map<ValueId, BasicBlock> blocks;
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
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::FunctionArguments>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::FunctionArguments;
    using StorageType   = SparseStorage<snir::v2::FunctionArguments>;
};

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::FunctionBody>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::FunctionBody;
    using StorageType   = SparseStorage<snir::v2::FunctionBody>;
};
