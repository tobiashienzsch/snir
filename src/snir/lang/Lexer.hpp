#pragma once

#include "snir/lang/ParseContext.hpp"
#include "snir/lang/Token.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cctype>
#include <cstddef>
#include <string_view>

namespace snir::lang {

struct Lexer
{
    Lexer(ParseContext& ctx, std::string_view text) : _context{&ctx}, _text{text} {}

    auto nextToken() -> SyntaxToken
    {
        if (_position >= _text.size()) {
            return SyntaxToken{
                .type     = SyntaxTokenType::EndOfFile,
                .position = _position,
                .text     = std::string(1, '\0'),
            };
        }
        // whitespace
        if (std::isspace(current()) != 0) {
            auto const start = _position;
            while (std::isspace(current()) != 0) {
                advance();
            }

            auto const length = _position - start;

            return SyntaxToken{
                .type     = SyntaxTokenType::Whitespace,
                .position = start,
                .text     = std::string{_text.substr(start, length)},
            };
        }
        // literal_integer
        if (std::isdigit(current()) != 0) {
            auto const start = _position;
            while (std::isdigit(current()) != 0) {
                advance();
            }

            auto const length = _position - start;
            return SyntaxToken{
                .type     = SyntaxTokenType::LiteralInteger,
                .position = start,
                .text     = std::string{_text.substr(start, length)},
            };
        }

        if (current() == '+') {
            return {.type = SyntaxTokenType::Plus, .position = _position++, .text = "+"};
        }

        if (current() == '-') {
            return {.type = SyntaxTokenType::Minus, .position = _position++, .text = "-"};
        }

        if (current() == '*') {
            return {.type = SyntaxTokenType::Star, .position = _position++, .text = "*"};
        }

        if (current() == '/') {
            return {.type = SyntaxTokenType::Slash, .position = _position++, .text = "/"};
        }

        if (current() == '(') {
            return {.type = SyntaxTokenType::OpenBrace, .position = _position++, .text = "("};
        }

        if (current() == ')') {
            return {.type = SyntaxTokenType::CloseBrace, .position = _position++, .text = ")"};
        }

        _context->addError(fmt::format("bad character input: {0}", current()));

        return SyntaxToken{
            .type     = SyntaxTokenType::BadToken,
            .position = _position++,
            .text     = std::string{_text.substr(_position - 1, 1)},
        };
    }

private:
    [[nodiscard]] auto current() const noexcept -> char
    {
        return _position >= _text.size() ? '\0' : _text[_position];
    }

    auto advance() noexcept -> std::size_t { return ++_position; }

    ParseContext* _context;
    std::string_view _text;
    std::size_t _position{0};
};

}  // namespace snir::lang
