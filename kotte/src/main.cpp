#include "kotte/application.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>
#include <string_view>

namespace
{
    bool has_argument(int argc, char** argv, std::string_view name){
        const std::span<char*> arguments{argv, static_cast<std::size_t>(argc)};
        for(const char* argument : arguments){
            if(std::string_view{argument} == name){
                return true;
            }
        }
        return false;
    }
}

int main(int argc, char** argv){
    try{
        constexpr std::uint64_t seed = 0x4b4f545445ULL;
        const kotte::WorldLayout layout = has_argument(argc, argv, "--stress")
            ? kotte::WorldLayout::stress
            : kotte::WorldLayout::gameplay;
        const bool trace_bombs = has_argument(argc, argv, "--trace-bombs");
        kotte::Application app{1280, 720, "Kotte", seed, layout, 0, trace_bombs};
        app.run();
    } catch(const std::exception& error){
        TraceLog(LOG_ERROR, "%s", error.what());
        return 1;
    }
    return 0;
}
