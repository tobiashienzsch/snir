#undef NDEBUG

#include "snir/lang/Parser.hpp"

#include <fmt/format.h>
#include <fmt/os.h>

#include <array>
#include <cassert>

auto main() -> int
{
    fmt::print("parser: constant\n");
    for (auto const source : std::array{"42", "143", "1", "0"}) {
        auto p   = Parser(source);
        auto ast = p.generateAst();
        assert(ast->getType() == AstNodeType::Constant);
        assert(ast->getChildren().empty());
    }

    fmt::print("parser: binary_expression\n");
    {
        fmt::print("\tplus\n");
        for (auto const source : std::array{"42+2", "143+2"}) {
            auto p   = Parser(source);
            auto ast = p.generateAst();
            assert(ast->getType() == AstNodeType::BinaryExpression);
            assert(ast->getChildren().at(0)->getType() == AstNodeType::Constant);
            assert(ast->getChildren().at(1)->getType() == AstNodeType::Op);
            assert(ast->getChildren().at(1)->getSource() == std::string("+"));
            assert(ast->getChildren().at(2)->getType() == AstNodeType::Constant);
        }

        fmt::print("\tminus\n");
        for (auto const source : std::array{"42-2", "143-2"}) {
            auto p   = Parser(source);
            auto ast = p.generateAst();
            assert(ast->getType() == AstNodeType::BinaryExpression);
            assert(ast->getChildren().at(0)->getType() == AstNodeType::Constant);
            assert(ast->getChildren().at(1)->getType() == AstNodeType::Op);
            assert(ast->getChildren().at(1)->getSource() == std::string("-"));
            assert(ast->getChildren().at(2)->getType() == AstNodeType::Constant);
        }
    }

    return EXIT_SUCCESS;
}
