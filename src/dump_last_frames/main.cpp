#include "command_line.hpp"
#include "dump_frames.hpp"

int main(int argc, const char* argv[])
{
    const CommandLine cli({argv, static_cast<std::size_t>(argc)});

    return dump_frames(cli.filename(), cli.use_threads());
}
