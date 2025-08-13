#undef NDEBUG

#include "snir/lang/Lexer.hpp"

#include <array>
#include <cassert>

auto main() -> int
{
    fmt::print("Lexer: operator\n");
    {
        auto ctx = ParseContext{};
        assert(Lexer(ctx, "+").nextToken().type == SyntaxTokenType::Plus);
        assert(Lexer(ctx, "-").nextToken().type == SyntaxTokenType::Minus);
        assert(Lexer(ctx, "*").nextToken().type == SyntaxTokenType::Star);
        assert(Lexer(ctx, "/").nextToken().type == SyntaxTokenType::Slash);
        assert(Lexer(ctx, "(").nextToken().type == SyntaxTokenType::OpenBrace);
        assert(Lexer(ctx, ")").nextToken().type == SyntaxTokenType::CloseBrace);
        assert(not ctx.hasErrors());
    }

    fmt::print("Lexer: NextToken(bad_token)\n");
    for (auto const source : std::array{"@", "#", "%", "&", "~"}) {
        auto ctx = ParseContext{};
        auto lex = Lexer(ctx, source);

        auto token = lex.nextToken();
        assert(token.type == SyntaxTokenType::BadToken);
        assert(token.position == 0);
        assert(token.text.size() == 1);

        assert(ctx.hasErrors());
    }

    fmt::print("Lexer: NextToken\n");
    {
        fmt::print("No spaces\n");
        for (auto const source : std::array{"42+2", "12+3", "99+0"}) {

            auto ctx = ParseContext{};
            auto lex = Lexer(ctx, source);

            auto token = lex.nextToken();
            assert(token.type == SyntaxTokenType::LiteralInteger);
            assert(token.position == 0);
            assert(token.text.size() == 2);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::Plus);
            assert(token.position == 2);
            assert(token.text.size() == 1);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::LiteralInteger);
            assert(token.position == 3);
            assert(token.text.size() == 1);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::EndOfFile);
            assert(token.position == 4);
            assert(token.text.size() == 1);

            assert(not ctx.hasErrors());
        }

        fmt::print("With spaces\n");
        for (auto const source : std::array{" 42 + 2", " 12 + 3"}) {

            auto ctx = ParseContext{};
            auto lex = Lexer(ctx, source);

            auto token = lex.nextToken();
            assert(token.type == SyntaxTokenType::Whitespace);
            assert(token.position == 0);
            assert(token.text.size() == 1);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::LiteralInteger);
            assert(token.position == 1);
            assert(token.text.size() == 2);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::Whitespace);
            assert(token.position == 3);
            assert(token.text.size() == 1);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::Plus);
            assert(token.position == 4);
            assert(token.text.size() == 1);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::Whitespace);
            assert(token.position == 5);
            assert(token.text.size() == 1);

            token = lex.nextToken();
            assert(token.type == SyntaxTokenType::LiteralInteger);
            assert(token.position == 6);
            assert(token.text.size() == 1);

            assert(not ctx.hasErrors());
        }
    }

    return EXIT_SUCCESS;
}
