#include "Ast.hpp"

auto operator<<(std::ostream& out, AstNodeType const type) -> std::ostream&
{
    switch (type) {
        case AstNodeType::Op: {
            out << "Operator";
            break;
        }
        case AstNodeType::Constant: {
            out << "Constant";
            break;
        }
        case AstNodeType::BinaryExpression: {
            out << "BinaryExpr";
            break;
        }
        case AstNodeType::UnaryExpression: {
            out << "UnaryExpr";
            break;
        }
        case AstNodeType::BracedExpression: {
            out << "BracedExpr";
            break;
        }
    }
    return out;
}
