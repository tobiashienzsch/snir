#include "snir/ir/Parser.hpp"
#include "snir/core/File.hpp"
#include "snir/core/Print.hpp"
#include "snir/core/Strings.hpp"
#include "snir/ir/Branch.hpp"
#include "snir/ir/CompareKind.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Instruction.hpp"
#include "snir/ir/Literal.hpp"

#include <ctre.hpp>

#undef NDEBUG
#include <cassert>

using namespace snir;

#define CHECK_CONTAINS(str, sub)                                                                     \
    do {                                                                                             \
        if (not ::snir::strings::contains((str), (sub))) {                                           \
            ::snir::raisef<std::runtime_error>("'{}' does not contain '{}'", (str), (sub));          \
        }                                                                                            \
    } while (false)

#define CHECK_THROW_CONTAINS(stmt, msg)                                                              \
    do {                                                                                             \
        auto didRun = false;                                                                         \
        try {                                                                                        \
            (void)((stmt));                                                                          \
            didRun = true;                                                                           \
        } catch ([[maybe_unused]] ::std::exception const& e) {                                       \
            CHECK_CONTAINS(e.what(), (msg));                                                         \
        }                                                                                            \
        assert(not didRun);                                                                          \
    } while (false)

namespace {

[[nodiscard]] auto parseErrorMessage(std::string_view source) -> std::string_view
{
    auto spec = snir::strings::getBetween(source, "; BEGIN_TEST", "; END_TEST");

    auto const tag       = std::string_view{"error: "};
    auto const tagPos    = spec.find(tag);
    auto const msgPos    = tagPos + tag.size();
    auto const endOfLine = spec.find('\n', tagPos);
    auto const error     = spec.substr(msgPos, endOfLine - msgPos);
    return error;
}

auto testTypeParser() -> void
{
    assert(parseType("void") == Type::Void);
    assert(parseType("ptr") == Type::Pointer);
    assert(parseType("i1") == Type::Bool);
    assert(parseType("i64") == Type::Int64);
    assert(parseType("float") == Type::Float);
    assert(parseType("double") == Type::Double);
    CHECK_THROW_CONTAINS(parseType("bool"), "failed to parse 'bool' as Type");
}

auto testCompareKindParser() -> void
{
    assert(parseCompareKind("eq") == CompareKind::Equal);
    assert(parseCompareKind("ne") == CompareKind::NotEqual);
    CHECK_THROW_CONTAINS(parseCompareKind("long"), "failed to parse 'long' as CompareKind");
}

auto testLiteralParser() -> void
{
    assert(std::get<int64_t>(parseLiteral("143", Type::Int64).value) == 143);
    assert(std::get<float>(parseLiteral("1.25", Type::Float).value) == 1.25F);
    assert(std::get<double>(parseLiteral("1.25", Type::Double).value) == 1.25);
    CHECK_THROW_CONTAINS(parseLiteral("bool", Type::Bool), "failed to parse 'bool' as Literal");
}

auto testIdentifierParser() -> void
{
    assert(parseIdentifier(R"(%foo)").first == IdentifierKind::Local);
    assert(parseIdentifier(R"(%foo)").second == "foo");
    assert(parseIdentifier(R"(@bar)").first == IdentifierKind::Global);
    assert(parseIdentifier(R"(@bar)").second == "bar");
    CHECK_THROW_CONTAINS(parseIdentifier(R"(&foo)"), "failed to parse '&foo' as Identifier");
}

auto testInstKindParser() -> void
{
    assert(parseInstKind("add") == InstKind::Add);
    assert(parseInstKind("sub") == InstKind::Sub);
    assert(parseInstKind("mul") == InstKind::Mul);
    assert(parseInstKind("div") == InstKind::Div);

    assert(parseInstKind("fadd") == InstKind::FloatAdd);
    assert(parseInstKind("fsub") == InstKind::FloatSub);
    assert(parseInstKind("fmul") == InstKind::FloatMul);
    assert(parseInstKind("fdiv") == InstKind::FloatDiv);

    CHECK_THROW_CONTAINS(parseInstKind("foo"), "failed to parse 'foo' as InstKind");
}

auto testParser() -> void
{
    auto registry = Registry{};
    auto parser   = Parser{registry};

    auto branch = registry.view<snir::Branch>();

    auto const* file = "./test/files/i64_blocks.ll";

    auto const source = readFile(file).value();
    auto const module = parser.read(source);

    auto const func    = Function{Value(registry, module.functions().at(0))};
    auto const& blocks = func.basicBlocks();
    assert(blocks.size() == 4);

    auto const terminatorVal = Value{registry, blocks.at(0).instructions.back()};
    auto const terminator    = Instruction{terminatorVal};
    assert(terminator.isTerminator());
    assert(terminator.kind() == InstKind::Branch);
    assert(terminator.type() == Type::Bool);

    auto const [br] = branch.get(terminator);
    assert(br.iftrue == blocks.at(1).label);
}

auto testParserErrors() -> void
{
    auto registry = Registry{};
    auto parser   = Parser{registry};
    for (auto const& entry : std::filesystem::directory_iterator{"./test/files/error"}) {
        snir::println("; {}", entry.path().string());
        auto const source = readFile(entry).value();
        auto const error  = parseErrorMessage(source);
        CHECK_THROW_CONTAINS(parser.read(source), error);
    }
}

}  // namespace

auto main() -> int
{
    testTypeParser();
    testCompareKindParser();
    testLiteralParser();
    testIdentifierParser();
    testInstKindParser();
    testParser();
    testParserErrors();
    return EXIT_SUCCESS;
}
