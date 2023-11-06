#include "Parser.hpp"

#include "Print.hpp"

#include <regex>

namespace snir {

auto Parser::operator()(std::string const& source) -> std::optional<Module>
{
    auto pattern = std::regex(R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})");
    auto search  = std::string::const_iterator(source.cbegin());
    auto matches = std::smatch{};

    while (std::regex_search(search, source.cend(), matches, pattern)) {
        auto const type      = matches[1].str();
        auto const name      = matches[2].str();
        auto const arguments = matches[3].str();
        auto const body      = matches[4].str();
        std::println("define {} {}({}) {{{}}}\n", type, name, arguments, body);
        search = matches.suffix().first;  // Update the search start position
    }

    return std::nullopt;
}

}  // namespace snir
