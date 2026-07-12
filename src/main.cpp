//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#include "trace/options.hpp"
#include "trace/tracer.hpp"

#include <exception>
#include <iostream>

int main(const int argc, char* argv[]) {
    try
    {
        const trace::options::ParseResult parseResult = trace::options::parse(argc, argv);
        if (parseResult.status != trace::options::ParseStatus::Ok)
        {
            trace::options::print_error(parseResult, argc, argv);

            if (parseResult.status == trace::options::ParseStatus::HelpRequested)
            {
                return 0;
            }

            return 1;
        }

        trace::Tracer tracer(parseResult);
        if (tracer.run() != 0)
        {
            std::cerr << "tracer.run() error" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
