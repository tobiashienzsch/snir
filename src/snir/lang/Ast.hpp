#pragma once

#include "snir/lang/Token.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace snir::lang {

enum struct AstNodeType : std::uint8_t
{
    Op,
    Constant,
    BinaryExpression,
    UnaryExpression,
    BracedExpression,
};

auto operator<<(std::ostream& out, AstNodeType type) -> std::ostream&;

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct AstNode
{
    AstNode()          = default;
    virtual ~AstNode() = default;

    AstNode(AstNode const&)                        = delete;
    AstNode(AstNode&&) noexcept                    = delete;
    auto operator=(AstNode const&) -> AstNode&     = delete;
    auto operator=(AstNode&&) noexcept -> AstNode& = delete;

    [[nodiscard]] virtual auto getType() const -> AstNodeType                                   = 0;
    [[nodiscard]] virtual auto getSource() const -> std::string                                 = 0;
    [[nodiscard]] virtual auto getChildren() const -> std::vector<AstNode*>                     = 0;
    [[nodiscard]] virtual auto getSourceLocation() const -> std::pair<std::size_t, std::size_t> = 0;
};

struct AstOperator final : public AstNode
{
    explicit AstOperator(SyntaxToken token) : _token{std::move(token)} {}

    ~AstOperator() override = default;

    [[nodiscard]] auto getType() const -> AstNodeType override { return AstNodeType::Op; }

    [[nodiscard]] auto getChildren() const -> std::vector<AstNode*> override { return {}; }

    [[nodiscard]] auto getSource() const -> std::string override { return _token.text; }

    [[nodiscard]] auto getSourceLocation() const -> std::pair<std::size_t, std::size_t> override
    {
        return std::make_pair(_token.position, _token.position + _token.text.size());
    }

    [[nodiscard]] auto token() const noexcept -> SyntaxToken { return _token; }

private:
    SyntaxToken _token;
};

struct AstConstant final : public AstNode
{
    explicit AstConstant(SyntaxToken token) : _token{std::move(token)} {}

    ~AstConstant() override = default;

    [[nodiscard]] auto getType() const -> AstNodeType override { return AstNodeType::Constant; }

    [[nodiscard]] auto getChildren() const -> std::vector<AstNode*> override { return {}; }

    [[nodiscard]] auto getSource() const -> std::string override { return _token.text; }

    [[nodiscard]] auto getSourceLocation() const -> std::pair<std::size_t, std::size_t> override
    {
        return std::make_pair(_token.position, _token.position + _token.text.size());
    }

    [[nodiscard]] auto token() const noexcept -> SyntaxToken { return _token; }

private:
    SyntaxToken _token;
};

struct AstBinaryExpr final : public AstNode
{
    AstBinaryExpr(std::unique_ptr<AstNode> lhs, SyntaxToken const& op, std::unique_ptr<AstNode> rhs)
        : _lhs{std::move(lhs)}
        , _operator{std::make_unique<AstOperator>(op)}
        , _rhs{std::move(rhs)}
    {}

    ~AstBinaryExpr() override = default;

    [[nodiscard]] auto getType() const -> AstNodeType override
    {
        return AstNodeType::BinaryExpression;
    }

    [[nodiscard]] auto getChildren() const -> std::vector<AstNode*> override
    {
        auto res = std::vector<AstNode*>{_lhs.get(), _operator.get(), _rhs.get()};
        return res;
    }

    [[nodiscard]] auto getSource() const -> std::string override
    {
        return fmt::format("{} {} {}", _lhs->getSource(), _operator->getSource(), _rhs->getSource());
    }

    [[nodiscard]] auto getSourceLocation() const -> std::pair<std::size_t, std::size_t> override
    {
        auto const start = _lhs->getSourceLocation().first;
        auto const end   = _rhs->getSourceLocation().second;
        return std::make_pair(start, end);
    }

private:
    std::unique_ptr<AstNode> _lhs;
    std::unique_ptr<AstOperator> _operator;
    std::unique_ptr<AstNode> _rhs;
};

struct AstUnaryExp final : public AstNode
{
    AstUnaryExp(SyntaxToken const& op, std::unique_ptr<AstNode> operand)
        : _operator{std::make_unique<AstOperator>(op)}
        , _operand{std::move(operand)}
    {}

    ~AstUnaryExp() override = default;

    [[nodiscard]] auto getType() const -> AstNodeType override
    {
        return AstNodeType::UnaryExpression;
    }

    [[nodiscard]] auto getChildren() const -> std::vector<AstNode*> override
    {
        auto res = std::vector<AstNode*>{_operator.get(), _operand.get()};
        return res;
    }

    [[nodiscard]] auto getSource() const -> std::string override
    {
        return fmt::format("{} {}", _operator->getSource(), _operand->getSource());
    }

    [[nodiscard]] auto getSourceLocation() const -> std::pair<std::size_t, std::size_t> override
    {
        auto const start = _operator->getSourceLocation().first;
        auto const end   = _operand->getSourceLocation().second;
        return std::make_pair(start, end);
    }

private:
    std::unique_ptr<AstOperator> _operator;
    std::unique_ptr<AstNode> _operand;
};

struct AstBracedExpr final : public AstNode
{
    AstBracedExpr(SyntaxToken open, std::unique_ptr<AstNode> exp, SyntaxToken close)
        : _open{std::move(open)}
        , _expression{std::move(exp)}
        , _close{std::move(close)}
    {}

    ~AstBracedExpr() override = default;

    [[nodiscard]] auto getType() const -> AstNodeType override
    {
        return AstNodeType::BracedExpression;
    }

    [[nodiscard]] auto getChildren() const -> std::vector<AstNode*> override
    {
        return std::vector<AstNode*>{_expression.get()};
    }

    [[nodiscard]] auto getSource() const -> std::string override
    {
        return fmt::format("({})", _expression->getSource());
    }

    [[nodiscard]] auto getSourceLocation() const -> std::pair<std::size_t, std::size_t> override
    {
        auto const start = _open.position;
        auto const end   = _close.position + 1;
        return std::make_pair(start, end);
    }

private:
    SyntaxToken _open;
    std::unique_ptr<AstNode> _expression;
    SyntaxToken _close;
};

struct AstUtils
{
    static auto
    prettyPrint(std::ostream& out, AstNode const& node, std::string indent = "", bool isLast = true)
        -> void
    {
        auto const marker = isLast ? std::string("└── ") : std::string("├── ");

        auto location = node.getSourceLocation();
        out << fmt::format(
            "{0}{1}<{2}[{3}, {4})>\n",
            indent,
            marker,
            fmt::streamed(node.getType()),
            location.first,
            location.second
        );

        indent += isLast ? std::string("    ") : std::string("│   ");

        if (auto const children = node.getChildren(); not children.empty()) {
            std::for_each(children.begin(), children.end() - 1, [&](auto const* const child) {
                prettyPrint(out, *child, indent, false);
            });

            prettyPrint(out, *children.back(), indent, true);
        }
    }
};

}  // namespace snir::lang
