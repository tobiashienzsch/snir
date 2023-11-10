#pragma once

#include <cstdint>

namespace snir::v2 {

enum struct ValueKind : std::uint8_t
{
    Register,
    Label,
    Literal,
    Function,
};

}  // namespace snir::v2
