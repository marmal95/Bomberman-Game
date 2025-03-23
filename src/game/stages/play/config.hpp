#pragma once

#include <iostream>
#include <toml++/impl/parse_error.hpp>
#include <toml++/toml.hpp>

namespace config
{
struct Config
{
    const bool playersCollidable{};
    const bool chainExplosion{};
};

inline std::ostream& operator<<(std::ostream& out, const Config& config)
{
    const auto c = std::make_tuple(std::make_pair("playersCollidable", config.playersCollidable),
                                   std::make_pair("chainExplosion", config.chainExplosion));

    out << "Config{";
    std::apply(
        [&](auto&&... field) {
            bool first = true;
            ((out << (first ? "" : ", ") << field.first << ": " << field.second, first = false), ...);
        },
        c);

    out << "}";
    return out;
}

inline auto parse()
{
    try
    {
        auto config = toml::parse_file("configuration.toml");
        auto result = Config{.playersCollidable = config["game"]["players_collidable"].value<bool>().value_or(false),
                             .chainExplosion = config["game"]["chain_explosion"].value<bool>().value_or(false)};
        std::cout << "Config: " << result << std::endl;
        return result;
    }
    catch (const toml::parse_error& e)
    {
        std::cerr << "Error reading configuration: " << e.what() << std::endl;
        std::cerr << "Continuing with default configuration!" << std::endl;
    }

    return Config{};
}

}