#pragma once

#include "snir/lang/Ast.hpp"
#include "snir/lang/Lexer.hpp"
#include "snir/lang/ParseContext.hpp"

#include <algorithm>
#include <memory>

struct Parser
{
    explicit Parser(std::string_view text)
    {
        auto lex   = Lexer{_context, text};
        auto token = lex.nextToken();
        while (token.type != SyntaxTokenType::EndOfFile) {
            if (token.type != SyntaxTokenType::Whitespace
                && token.type != SyntaxTokenType::BadToken) {
                _tokens.push_back(token);
            }
            token = lex.nextToken();
        }
        _tokens.push_back(token);
    }

    [[nodiscard]] auto generateAst() -> std::unique_ptr<AstNode>
    {
        auto exp = parseExpression();
        auto eof = matchToken(SyntaxTokenType::EndOfFile);
        return exp;
    }

    auto printDiagnostics(std::ostream& out) const -> void { _context.printErrors(out); }

    [[nodiscard]] auto getTokens() const noexcept -> std::vector<SyntaxToken> const&
    {
        return _tokens;
    }

private:
    [[nodiscard]] auto parseExpression(int parentPrecedence = 0) -> std::unique_ptr<AstNode>
    {
        auto left            = std::unique_ptr<AstNode>();
        auto unaryPrecedence = getUnaryOpPrecedence(current().type);
        if (unaryPrecedence != 0 && unaryPrecedence >= parentPrecedence) {
            auto opToken = nextToken();
            auto operand = parseExpression(unaryPrecedence);
            left         = std::make_unique<AstUnaryExp>(opToken, std::move(operand));
        } else {
            left = parsePrimaryExpression();
        }

        while (true) {
            auto const precedence = getBinaryOpPrecedence(current().type);
            if (precedence == 0 || precedence <= parentPrecedence) {
                break;
            }

            auto opToken = nextToken();
            auto right   = parseExpression(precedence);
            left = std::make_unique<AstBinaryExpr>(std::move(left), opToken, std::move(right));
        }

        return left;
    }

    [[nodiscard]] auto parsePrimaryExpression() -> std::unique_ptr<AstNode>
    {

        if (current().type == SyntaxTokenType::OpenBrace) {
            auto left       = nextToken();
            auto expression = parseExpression();
            auto right      = matchToken(SyntaxTokenType::CloseBrace);
            return std::make_unique<AstBracedExpr>(left, std::move(expression), right);
        }

        auto numberToken = matchToken(SyntaxTokenType::LiteralInteger);
        return std::make_unique<AstConstant>(numberToken);
    }

    [[nodiscard]] auto matchToken(SyntaxTokenType type) noexcept -> SyntaxToken
    {
        if (current().type == type) {
            return nextToken();
        }

        _context.addError(
            fmt::format(
                "at {0} unexpected token <{1}>, expected <{2}>",
                current().position,
                fmt::streamed(current().type),
                fmt::streamed(type)
            )
        );
        return SyntaxToken{.type = type, .position = current().position, .text = ""};
    }

    [[nodiscard]] auto nextToken() noexcept -> SyntaxToken
    {
        auto cur = current();
        _position++;
        return cur;
    }

    [[nodiscard]] auto peek(size_t offset) const noexcept -> SyntaxToken
    {
        auto const index = _position + offset;
        if (index >= _tokens.size()) {
            return _tokens.back();
        }
        return _tokens.at(index);
    }

    [[nodiscard]] auto current() const noexcept -> SyntaxToken { return peek(0); }

    ParseContext _context;
    std::size_t _position{};
    std::vector<SyntaxToken> _tokens;
};
