#include "Token.hpp"

namespace snir::lang {
auto operator<<(std::ostream& out, SyntaxTokenType const type) -> std::ostream&
{
    switch (type) {
#define TCC_TOKEN_TYPE(x)                                                                            \
    case SyntaxTokenType::x: {                                                                       \
        out << #x;                                                                                   \
        break;                                                                                       \
    }
        TCC_TOKENS
#undef TCC_TOKEN_TYPE
    }

    return out;
}
}  // namespace snir::lang
