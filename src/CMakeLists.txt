cmake_minimum_required(VERSION 3.27)
project(bomberman VERSION 1.0 LANGUAGES CXX)

include(common)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

find_package(EnTT CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

add_executable(bomberman
    main.cpp
    game/game.cpp
    game/game_manager.cpp
    game/stages/stage.cpp
    game/stages/menu/menu_stage.cpp
    game/stages/settings/settings_stage.cpp
    game/stages/play/collision_detector.cpp
    game/stages/play/entity_creator.cpp
    game/stages/play/map_creator.cpp
    game/stages/play/play_stage.cpp
    game/stages/play/systems/animate_system.cpp
    game/stages/play/systems/collision_system.cpp
    game/stages/play/systems/explosion_system.cpp
    game/stages/play/systems/move_system.cpp
    game/stages/play/systems/render_system.cpp
    game/stages/play/systems/spawn_system.cpp
    game/stages/exit/exit_stage.cpp
    )
set_common_target_compile_properties(bomberman)
target_link_libraries(bomberman PRIVATE EnTT::EnTT)
target_link_libraries(bomberman PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio)
target_link_libraries(bomberman PRIVATE PkgConfig::tomlplusplus)
target_include_directories(bomberman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (DEBUG_MODE)
    target_compile_definitions(DEBUG_MODE=1)
endif()