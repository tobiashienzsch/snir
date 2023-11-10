#pragma once

#include "snir/ecs/handle.hpp"
#include "snir/ecs/storage.hpp"
#include "snir/ir/v2/type.hpp"
#include "snir/ir/v2/value_id.hpp"
#include "snir/ir/v2/value_kind.hpp"
#include "snir/ir/v2/value_registry.hpp"

#include <variant>
#include <vector>

namespace snir::v2 {

using Value = snir::Handle<ValueId>;

struct Name
{
    std::string text;
};

struct FunctionArgs
{
    std::vector<Type> args;
};

struct Literal
{
    std::variant<bool, std::int64_t, float, double> value;
};

}  // namespace snir::v2

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::Name>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::Name;
    using StorageType   = SparseStorage<snir::v2::Name>;
};

template<>
struct snir::StorageTraits<snir::v2::ValueId, snir::v2::Literal>
{
    using IdType        = snir::v2::ValueId;
    using ComponentType = snir::v2::Literal;
    using StorageType   = SparseStorage<snir::v2::Literal>;
};
