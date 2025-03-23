#pragma once

#include <iostream>
#include <string_view>
#include <toml++/impl/parse_error.hpp>
#include <toml++/toml.hpp>

namespace config
{
using Key = std::string_view;

namespace keys
{
constexpr Key GAME = "game";
constexpr Key PLAYERS_COLLIDABLE = "players_collidable";
constexpr Key CHAIN_EXPLOSION = "chain_explosion";
}

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
        auto result =
            Config{.playersCollidable = config[keys::GAME][keys::PLAYERS_COLLIDABLE].value<bool>().value_or(false),
                   .chainExplosion = config[keys::GAME][keys::CHAIN_EXPLOSION].value<bool>().value_or(false)};
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

inline void save(const Config& config)
{
    std::cout << "Updated config: " << config << std::endl;
}

}