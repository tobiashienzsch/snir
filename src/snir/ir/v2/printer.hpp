#pragma once

#include "snir/ir/v2/module.hpp"
#include "snir/ir/v2/value.hpp"

#include <ostream>

namespace snir::v2 {

struct Printer
{
    explicit Printer(std::ostream& out) : _out{out} {}

    auto operator()(Module& module) -> void
    {
        auto& vals = module.getRegistry().getValueRegistry();
        auto view  = vals.view<Type, Name, FuncArguments, FuncBody>();

        for (auto funcId : module.getFunctions()) {
            auto func = Value{vals, funcId};

            auto const [type, name, args, body] = view.get(func.getId());
            println(
                _out,
                "define {} @{}({}) {{{}}}",
                type,
                name.text,
                args.args.size(),
                body.blocks.size()
            );
        }
    }

private:
    std::reference_wrapper<std::ostream> _out;
};

}  // namespace snir::v2
