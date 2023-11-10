#pragma once

#include "snir/core/static_vector.hpp"
#include "snir/ecs/handle.hpp"
#include "snir/ir/v2/inst_id.hpp"
#include "snir/ir/v2/inst_kind.hpp"
#include "snir/ir/v2/inst_registry.hpp"
#include "snir/ir/v2/value_id.hpp"

namespace snir::v2 {

using Inst = Handle<InstId>;

struct Result
{
    ValueId id;
};

struct Operands
{
    StaticVector<ValueId, 2> list;
};

struct Branch
{
    std::optional<ValueId> condition;
    ValueId iftrue;
    std::optional<ValueId> iffalse;
};

}  // namespace snir::v2

template<>
struct snir::StorageTraits<snir::v2::InstId, snir::v2::Branch>
{
    using IdType        = snir::v2::InstId;
    using ComponentType = snir::v2::Branch;
    using StorageType   = SparseStorage<snir::v2::Branch>;
};
