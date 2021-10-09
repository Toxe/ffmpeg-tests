#include <filesystem>
#include <string_view>

#include "error.hpp"

[[nodiscard]] std::string_view eval_args(int argc, char* argv[])
{
    if (argc < 2)
        die("missing filename");

    if (!std::filesystem::exists(argv[1]))
        die("file not found");

    return argv[1];
}

int main(int argc, char* argv[])
{
    std::string_view filename = eval_args(argc, argv);
}
