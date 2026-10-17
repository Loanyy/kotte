#include "kotte/application.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>
#include <string_view>

namespace
{
    // The small gameplay room is the default. Start the program with
    // "--stress" to get the large Week 6 world instead.
    kotte::WorldLayout layout_from_arguments(int argc, char** argv){
        const std::span<char*> arguments{argv, static_cast<std::size_t>(argc)};
        for(const char* argument : arguments){
            if(std::string_view{argument} == "--stress"){
                return kotte::WorldLayout::stress;
            }
        }
        return kotte::WorldLayout::gameplay;
    }
}

int main(int argc, char** argv){
    try{
        constexpr std::uint64_t seed = 0x4b4f545445ULL;
        const kotte::WorldLayout layout = layout_from_arguments(argc, argv);
        kotte::Application app{1280, 720, "Kotte", seed, layout};
        app.run();
    } catch(const std::exception& error){
        TraceLog(LOG_ERROR, "%s", error.what());
        return 1;
    }
    return 0;
}
