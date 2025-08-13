#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>

#define TCC_TOKENS                                                                                   \
    TCC_TOKEN_TYPE(BadToken)                                                                         \
    TCC_TOKEN_TYPE(Whitespace)                                                                       \
    TCC_TOKEN_TYPE(LiteralInteger)                                                                   \
    TCC_TOKEN_TYPE(Identifier)                                                                       \
    TCC_TOKEN_TYPE(Plus)                                                                             \
    TCC_TOKEN_TYPE(Minus)                                                                            \
    TCC_TOKEN_TYPE(Star)                                                                             \
    TCC_TOKEN_TYPE(Slash)                                                                            \
    TCC_TOKEN_TYPE(OpenBrace)                                                                        \
    TCC_TOKEN_TYPE(CloseBrace)                                                                       \
    TCC_TOKEN_TYPE(EndOfFile)

#define TCC_TOKEN_TYPE(x) x,
enum struct SyntaxTokenType : std::uint8_t
{
    TCC_TOKENS
};
#undef TCC_TOKEN_TYPE

auto operator<<(std::ostream& out, SyntaxTokenType type) -> std::ostream&;

struct SyntaxToken
{
    SyntaxTokenType type;
    std::size_t position;
    std::string text;
};

[[nodiscard]] constexpr auto getBinaryOpPrecedence(SyntaxTokenType type) noexcept -> int
{
    switch (type) {
        case SyntaxTokenType::Star:
        case SyntaxTokenType::Slash: {
            return 2;
        }

        case SyntaxTokenType::Plus:
        case SyntaxTokenType::Minus: {
            return 1;
        }

        default: {
            return 0;
        }
    }
}

[[nodiscard]] constexpr auto getUnaryOpPrecedence(SyntaxTokenType type) noexcept -> int
{
    switch (type) {
        case SyntaxTokenType::Plus:
        case SyntaxTokenType::Minus: {
            return 1;
        }

        default: {
            return 0;
        }
    }
}
