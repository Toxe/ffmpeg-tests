#include <filesystem>
#include <span>

#include "dump_frames.hpp"
#include "error.hpp"

[[nodiscard]] std::string_view eval_args(std::span<const char*> args)
{
    if (args.size() < 2)
        die("missing filename");

    if (!std::filesystem::exists(args[1]))
        die("file not found");

    return args[1];
}

int main(int argc, const char* argv[])
{
    const auto filename = eval_args({argv, static_cast<std::size_t>(argc)});
    return dump_frames(filename);
}
