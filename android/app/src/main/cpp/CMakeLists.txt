# WoWee Android CMakeLists.txt
# Built by the Android NDK via Gradle. Produces libwowee.so loaded by WoWeeActivity.java.
#
# This file mirrors the root CMakeLists.txt but with Android-specific adjustments:
#   - No FFmpeg (use AAudio / OpenSL ES instead — SDK audio APIs)
#   - No Unicorn Engine (x86 emulation — won't run on ARM)
#   - No AMD FSR2/FSR3 (desktop GPU features, not available on Android NDK)
#   - No X11 / desktop-specific dependencies
#   - SDL2 built from source (extern/SDL, downloaded in CI)

cmake_minimum_required(VERSION 3.22.1)
project(wowee_android VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Paths ---
# Root WoWee source directory (relative to android/app/src/main/cpp/)
set(WOWEE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)
set(WOWEE_SRC ${WOWEE_ROOT}/src)
set(WOWEE_INCLUDE ${WOWEE_ROOT}/include)
set(WOWEE_EXTERN ${WOWEE_ROOT}/extern)

# Android source directory
set(WOWEE_ANDROID_SRC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Build definitions ---
add_compile_definitions(
    WOWEE_ANDROID=1
    WOWEE_RELEASE_LOGGING
    $<$<CONFIG:Debug>:WOWEE_DEBUG>
)

# --- Build SDL2 from source (downloaded in CI to extern/SDL) ---
set(SDL_DIR ${WOWEE_ROOT}/extern/SDL)
if(NOT EXISTS ${SDL_DIR})
    message(FATAL_ERROR "SDL2 not found at ${SDL_DIR}. Run CI workflow or download SDL2 release tarball.")
endif()

# SDL2 static library for Android
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_ATOMIC ON CACHE BOOL "" FORCE)

# Android-specific: disable features we don't need
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)

add_subdirectory(${SDL_DIR} ${CMAKE_BINARY_DIR}/SDL2)

# --- Find Vulkan (Android NDK provides Vulkan headers + loader) ---
find_package(Vulkan REQUIRED)

# --- OpenSSL (prebuilt for Android) ---
# Downloaded by CI from KDAB android_openssl → extern/openssl-android
set(OPENSSL_PREBUILT ${WOWEE_EXTERN}/openssl-android)
if(EXISTS ${OPENSSL_PREBUILT})
    set(WOWEE_OPENSSL_INCLUDE ${OPENSSL_PREBUILT}/include)
    find_library(WOWEE_SSL_LIB ssl PATHS ${OPENSSL_PREBUILT}/lib NO_DEFAULT_PATH)
    find_library(WOWEE_CRYPTO_LIB crypto PATHS ${OPENSSL_PREBUILT}/lib NO_DEFAULT_PATH)
    if(WOWEE_SSL_LIB AND WOWEE_CRYPTO_LIB)
        set(WOWEE_HAS_OPENSSL TRUE)
        message(STATUS "OpenSSL: prebuilt from ${OPENSSL_PREBUILT}")
    endif()
endif()
if(NOT WOWEE_HAS_OPENSSL)
    set(WOWEE_OPENSSL_INCLUDE ${WOWEE_ANDROID_SRC}/openssl-stub)
    add_compile_definitions(WOWEE_NO_OPENSSL=1)
    set(WOWEE_OPENSSL_STUBBED TRUE)
    message(STATUS "OpenSSL: stubbed (prebuilt not found)")
endif()

# --- GLM (header-only) ---
set(GLM_DIR ${WOWEE_EXTERN}/glm)
if(NOT EXISTS ${GLM_DIR})
    # Try system GLM via NDK — Android NDK doesn't ship GLM, so fall back
    find_package(glm QUIET)
    if(NOT glm_FOUND)
        message(FATAL_ERROR "GLM not found. Add it to extern/glm or install.")
    endif()
endif()

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL GLM_FORCE_DEPTH_ZERO_TO_ONE)

# --- ZLIB (Android NDK provides this) ---
find_package(ZLIB REQUIRED)

# --- Threads ---
find_package(Threads REQUIRED)

# --- Lua 5.1 (vendored, same as desktop) ---
set(LUA_DIR ${WOWEE_EXTERN}/lua-5.1.5/src)
if(NOT EXISTS ${LUA_DIR})
    message(FATAL_ERROR "Vendored Lua not found at ${LUA_DIR}. Clone with --recurse-submodules.")
endif()

set(LUA_SOURCES
    ${LUA_DIR}/lapi.c    ${LUA_DIR}/lcode.c   ${LUA_DIR}/ldebug.c
    ${LUA_DIR}/ldo.c     ${LUA_DIR}/ldump.c   ${LUA_DIR}/lfunc.c
    ${LUA_DIR}/lgc.c     ${LUA_DIR}/llex.c    ${LUA_DIR}/lmem.c
    ${LUA_DIR}/lobject.c ${LUA_DIR}/lopcodes.c ${LUA_DIR}/lparser.c
    ${LUA_DIR}/lstate.c  ${LUA_DIR}/lstring.c ${LUA_DIR}/ltable.c
    ${LUA_DIR}/ltm.c     ${LUA_DIR}/lundump.c ${LUA_DIR}/lvm.c
    ${LUA_DIR}/lzio.c    ${LUA_DIR}/lauxlib.c ${LUA_DIR}/lbaselib.c
    ${LUA_DIR}/ldblib.c  ${LUA_DIR}/liolib.c  ${LUA_DIR}/lmathlib.c
    ${LUA_DIR}/loslib.c  ${LUA_DIR}/ltablib.c ${LUA_DIR}/lstrlib.c
    ${LUA_DIR}/linit.c
)

add_library(lua51 STATIC ${LUA_SOURCES})
set_target_properties(lua51 PROPERTIES LINKER_LANGUAGE C C_STANDARD 99 POSITION_INDEPENDENT_CODE ON)
target_include_directories(lua51 PUBLIC ${LUA_DIR})
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lua51 PRIVATE -w)
endif()

# --- vk-bootstrap ---
set(VK_BOOTSTRAP_DIR ${WOWEE_EXTERN}/vk-bootstrap)
if(NOT EXISTS ${VK_BOOTSTRAP_DIR})
    message(FATAL_ERROR "vk-bootstrap not found at ${VK_BOOTSTRAP_DIR}. Clone with --recurse-submodules.")
endif()

add_library(vk-bootstrap STATIC
    ${VK_BOOTSTRAP_DIR}/src/VkBootstrap.cpp
)
target_include_directories(vk-bootstrap PUBLIC ${VK_BOOTSTRAP_DIR}/src)
target_link_libraries(vk-bootstrap PUBLIC Vulkan::Vulkan)

# --- ImGui (Vulkan + SDL2 backends) ---
set(IMGUI_DIR ${WOWEE_EXTERN}/imgui)
if(NOT EXISTS ${IMGUI_DIR})
    message(FATAL_ERROR "ImGui not found at ${IMGUI_DIR}.")
endif()

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC SDL2-static Vulkan::Vulkan ${CMAKE_DL_LIBS})

# --- WoWee Core Sources ---
# Same source files as the root CMakeLists, minus platform-specific exclusions.
# We compile everything as a single shared library.

set(WOWEE_WORLD_SOURCES
    # Core
    ${WOWEE_SRC}/core/application.cpp
    ${WOWEE_SRC}/core/entity_spawner.cpp
    ${WOWEE_SRC}/core/entity_spawner_player.cpp
    ${WOWEE_SRC}/core/entity_spawner_processing.cpp
    ${WOWEE_SRC}/core/appearance_composer.cpp
    ${WOWEE_SRC}/core/helm_visual.cpp
    ${WOWEE_SRC}/core/world_loader.cpp
    ${WOWEE_SRC}/core/npc_interaction_callback_handler.cpp
    ${WOWEE_SRC}/core/audio_callback_handler.cpp
    ${WOWEE_SRC}/core/entity_spawn_callback_handler.cpp
    ${WOWEE_SRC}/core/animation_callback_handler.cpp
    ${WOWEE_SRC}/core/transport_callback_handler.cpp
    ${WOWEE_SRC}/core/world_entry_callback_handler.cpp
    ${WOWEE_SRC}/core/ui_screen_callback_handler.cpp
    ${WOWEE_SRC}/core/window.cpp
    ${WOWEE_SRC}/core/input.cpp
    ${WOWEE_SRC}/core/logger.cpp
    ${WOWEE_SRC}/core/memory_monitor.cpp
    ${WOWEE_SRC}/core/app_clock.cpp

    # Android-specific core (replaces config_paths.cpp, same GetExecutableDir)
    ${WOWEE_ANDROID_SRC}/android_config_paths.cpp
    # Link-time stubs for excluded modules (OpenSSL, FFmpeg-dependent audio)
    ${WOWEE_ANDROID_SRC}/android_link_stubs.cpp

    # Math
    ${WOWEE_SRC}/math/spline.cpp

    # Network
    ${WOWEE_SRC}/network/socket.cpp
    ${WOWEE_SRC}/network/packet.cpp
    ${WOWEE_SRC}/network/tcp_socket.cpp
    ${WOWEE_SRC}/network/world_socket.cpp

    # Auth
    ${WOWEE_SRC}/auth/auth_opcodes.cpp
    ${WOWEE_SRC}/auth/auth_packets.cpp
    ${WOWEE_SRC}/auth/pin_auth.cpp
    ${WOWEE_SRC}/auth/integrity.cpp
    ${WOWEE_SRC}/auth/rc4.cpp
    ${WOWEE_SRC}/auth/vanilla_crypt.cpp

    # Game
    ${WOWEE_SRC}/game/expansion_profile.cpp
    ${WOWEE_SRC}/game/opcode_table.cpp
    ${WOWEE_SRC}/game/update_field_table.cpp
    ${WOWEE_SRC}/game/game_handler.cpp
    ${WOWEE_SRC}/game/game_handler_packets.cpp
    ${WOWEE_SRC}/game/game_handler_callbacks.cpp
    ${WOWEE_SRC}/game/chat_handler.cpp
    ${WOWEE_SRC}/game/movement_handler.cpp
    ${WOWEE_SRC}/game/combat_handler.cpp
    ${WOWEE_SRC}/game/spell_handler.cpp
    ${WOWEE_SRC}/game/inventory_handler.cpp
    ${WOWEE_SRC}/game/social_handler.cpp
    ${WOWEE_SRC}/game/quest_handler.cpp
    ${WOWEE_SRC}/game/entity_controller.cpp
    ${WOWEE_SRC}/game/transport_manager.cpp
    ${WOWEE_SRC}/game/transport_path_repository.cpp
    ${WOWEE_SRC}/game/transport_clock_sync.cpp
    ${WOWEE_SRC}/game/transport_animator.cpp
    ${WOWEE_SRC}/game/world.cpp
    ${WOWEE_SRC}/game/player.cpp
    ${WOWEE_SRC}/game/entity.cpp
    ${WOWEE_SRC}/game/opcodes.cpp
    ${WOWEE_SRC}/game/world_packets.cpp
    ${WOWEE_SRC}/game/world_packets_social.cpp
    ${WOWEE_SRC}/game/world_packets_entity.cpp
    ${WOWEE_SRC}/game/world_packets_world.cpp
    ${WOWEE_SRC}/game/world_packets_economy.cpp
    ${WOWEE_SRC}/game/spline_packet.cpp
    ${WOWEE_SRC}/game/faction_hostility.cpp
    ${WOWEE_SRC}/game/packet_parsers_tbc.cpp
    ${WOWEE_SRC}/game/packet_parsers_classic.cpp
    ${WOWEE_SRC}/game/character.cpp
    ${WOWEE_SRC}/game/zone_manager.cpp
    ${WOWEE_SRC}/game/inventory.cpp

    # Audio (stub on Android — original sources excluded, stub removed)

    # Pipeline (asset loaders)
    ${WOWEE_SRC}/pipeline/blp_loader.cpp
    ${WOWEE_SRC}/pipeline/dbc_loader.cpp
    ${WOWEE_SRC}/pipeline/asset_manager.cpp
    ${WOWEE_SRC}/pipeline/asset_manifest.cpp
    ${WOWEE_SRC}/pipeline/loose_file_reader.cpp
    ${WOWEE_SRC}/pipeline/m2_loader.cpp
    ${WOWEE_SRC}/pipeline/wmo_loader.cpp
    ${WOWEE_SRC}/pipeline/adt_loader.cpp
    ${WOWEE_SRC}/pipeline/wdt_loader.cpp
    ${WOWEE_SRC}/pipeline/wowee_terrain_loader.cpp
    ${WOWEE_SRC}/pipeline/wowee_model.cpp
    ${WOWEE_SRC}/pipeline/wowee_model_fromm2.cpp
    ${WOWEE_SRC}/pipeline/wowee_building.cpp
    ${WOWEE_SRC}/pipeline/wowee_collision.cpp
    ${WOWEE_SRC}/pipeline/wowee_light.cpp
    ${WOWEE_SRC}/pipeline/wowee_weather.cpp
    ${WOWEE_SRC}/pipeline/wowee_world_map.cpp
    ${WOWEE_SRC}/pipeline/wowee_sound.cpp
    ${WOWEE_SRC}/pipeline/wowee_spawns.cpp
    ${WOWEE_SRC}/pipeline/wowee_items.cpp
    ${WOWEE_SRC}/pipeline/wowee_loot.cpp
    ${WOWEE_SRC}/pipeline/wowee_creatures.cpp
    ${WOWEE_SRC}/pipeline/wowee_quests.cpp
    ${WOWEE_SRC}/pipeline/wowee_objects.cpp
    ${WOWEE_SRC}/pipeline/wowee_factions.cpp
    ${WOWEE_SRC}/pipeline/wowee_locks.cpp
    ${WOWEE_SRC}/pipeline/wowee_skills.cpp
    ${WOWEE_SRC}/pipeline/wowee_spells.cpp
    ${WOWEE_SRC}/pipeline/wowee_achievements.cpp
    ${WOWEE_SRC}/pipeline/wowee_trainers.cpp
    ${WOWEE_SRC}/pipeline/wowee_gossip.cpp
    ${WOWEE_SRC}/pipeline/wowee_taxi.cpp
    ${WOWEE_SRC}/pipeline/wowee_talents.cpp
    ${WOWEE_SRC}/pipeline/wowee_maps.cpp
    ${WOWEE_SRC}/pipeline/wowee_chars.cpp
    ${WOWEE_SRC}/pipeline/wowee_tokens.cpp
    ${WOWEE_SRC}/pipeline/wowee_triggers.cpp
    ${WOWEE_SRC}/pipeline/wowee_titles.cpp
    ${WOWEE_SRC}/pipeline/wowee_events.cpp
    ${WOWEE_SRC}/pipeline/wowee_mounts.cpp
    ${WOWEE_SRC}/pipeline/wowee_battlegrounds.cpp
    ${WOWEE_SRC}/pipeline/wowee_mail.cpp
    ${WOWEE_SRC}/pipeline/wowee_gems.cpp
    ${WOWEE_SRC}/pipeline/wowee_guilds.cpp
    ${WOWEE_SRC}/pipeline/wowee_conditions.cpp
    ${WOWEE_SRC}/pipeline/wowee_pets.cpp
    ${WOWEE_SRC}/pipeline/wowee_auction.cpp
    ${WOWEE_SRC}/pipeline/wowee_channels.cpp
    ${WOWEE_SRC}/pipeline/wowee_cinematics.cpp
    ${WOWEE_SRC}/pipeline/wowee_glyphs.cpp
    ${WOWEE_SRC}/pipeline/wowee_vehicles.cpp
    ${WOWEE_SRC}/pipeline/wowee_holidays.cpp
    ${WOWEE_SRC}/pipeline/wowee_liquids.cpp
    ${WOWEE_SRC}/pipeline/wowee_animations.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_visuals.cpp
    ${WOWEE_SRC}/pipeline/wowee_world_state_ui.cpp
    ${WOWEE_SRC}/pipeline/wowee_player_conditions.cpp
    ${WOWEE_SRC}/pipeline/wowee_trade_skills.cpp
    ${WOWEE_SRC}/pipeline/wowee_creature_equipment.cpp
    ${WOWEE_SRC}/pipeline/wowee_item_sets.cpp
    ${WOWEE_SRC}/pipeline/wowee_game_tips.cpp
    ${WOWEE_SRC}/pipeline/wowee_companions.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_mechanics.cpp
    ${WOWEE_SRC}/pipeline/wowee_keybindings.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_schools.cpp
    ${WOWEE_SRC}/pipeline/wowee_lfg.cpp
    ${WOWEE_SRC}/pipeline/wowee_macros.cpp
    ${WOWEE_SRC}/pipeline/wowee_char_features.cpp
    ${WOWEE_SRC}/pipeline/wowee_pvp.cpp
    ${WOWEE_SRC}/pipeline/wowee_bags.cpp
    ${WOWEE_SRC}/pipeline/wowee_runes.cpp
    ${WOWEE_SRC}/pipeline/wowee_loading_screens.cpp
    ${WOWEE_SRC}/pipeline/wowee_item_suffixes.cpp
    ${WOWEE_SRC}/pipeline/wowee_combat_ratings.cpp
    ${WOWEE_SRC}/pipeline/wowee_unit_movement.cpp
    ${WOWEE_SRC}/pipeline/wowee_quest_sorts.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_ranges.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_cast_times.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_durations.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_cooldowns.cpp
    ${WOWEE_SRC}/pipeline/wowee_creature_families.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_power_costs.cpp
    ${WOWEE_SRC}/pipeline/wowee_glyph_slots.cpp
    ${WOWEE_SRC}/pipeline/wowee_creature_difficulties.cpp
    ${WOWEE_SRC}/pipeline/wowee_item_materials.cpp
    ${WOWEE_SRC}/pipeline/wowee_player_spawn_profiles.cpp
    ${WOWEE_SRC}/pipeline/wowee_talent_tabs.cpp
    ${WOWEE_SRC}/pipeline/wowee_currency_types.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_reagents.cpp
    ${WOWEE_SRC}/pipeline/wowee_achievement_criteria.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_effect_types.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_aura_types.cpp
    ${WOWEE_SRC}/pipeline/wowee_item_qualities.cpp
    ${WOWEE_SRC}/pipeline/wowee_skill_costs.cpp
    ${WOWEE_SRC}/pipeline/wowee_item_flags.cpp
    ${WOWEE_SRC}/pipeline/wowee_npc_services.cpp
    ${WOWEE_SRC}/pipeline/wowee_token_rewards.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_procs.cpp
    ${WOWEE_SRC}/pipeline/wowee_creature_patrols.cpp
    ${WOWEE_SRC}/pipeline/wowee_boss_encounters.cpp
    ${WOWEE_SRC}/pipeline/wowee_instance_lockouts.cpp
    ${WOWEE_SRC}/pipeline/wowee_stable_slots.cpp
    ${WOWEE_SRC}/pipeline/wowee_stat_curves.cpp
    ${WOWEE_SRC}/pipeline/wowee_action_bars.cpp
    ${WOWEE_SRC}/pipeline/wowee_group_compositions.cpp
    ${WOWEE_SRC}/pipeline/wowee_hearth_binds.cpp
    ${WOWEE_SRC}/pipeline/wowee_server_broadcasts.cpp
    ${WOWEE_SRC}/pipeline/wowee_combat_maneuvers.cpp
    ${WOWEE_SRC}/pipeline/wowee_realm_list.cpp
    ${WOWEE_SRC}/pipeline/wowee_emotes.cpp
    ${WOWEE_SRC}/pipeline/wowee_buff_book.cpp
    ${WOWEE_SRC}/pipeline/wowee_tabards.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_markers.cpp
    ${WOWEE_SRC}/pipeline/wowee_learning_notifications.cpp
    ${WOWEE_SRC}/pipeline/wowee_creature_resists.cpp
    ${WOWEE_SRC}/pipeline/wowee_pet_talents.cpp
    ${WOWEE_SRC}/pipeline/wowee_heroic_scaling.cpp
    ${WOWEE_SRC}/pipeline/wowee_reputation_rewards.cpp
    ${WOWEE_SRC}/pipeline/wowee_minimap_levels.cpp
    ${WOWEE_SRC}/pipeline/wowee_pet_care.cpp
    ${WOWEE_SRC}/pipeline/wowee_movie_credits.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_variants.cpp
    ${WOWEE_SRC}/pipeline/wowee_voiceovers.cpp
    ${WOWEE_SRC}/pipeline/wowee_trade_rules.cpp
    ${WOWEE_SRC}/pipeline/wowee_word_filters.cpp
    ${WOWEE_SRC}/pipeline/wowee_raid_markers.cpp
    ${WOWEE_SRC}/pipeline/wowee_loot_modes.cpp
    ${WOWEE_SRC}/pipeline/wowee_sky_params.cpp
    ${WOWEE_SRC}/pipeline/wowee_server_config.cpp
    ${WOWEE_SRC}/pipeline/wowee_anniversary_events.cpp
    ${WOWEE_SRC}/pipeline/wowee_pvp_ranks.cpp
    ${WOWEE_SRC}/pipeline/wowee_localization.cpp
    ${WOWEE_SRC}/pipeline/wowee_global_channels.cpp
    ${WOWEE_SRC}/pipeline/wowee_addon_manifest.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_pack.cpp
    ${WOWEE_SRC}/pipeline/wowee_player_movement_anim.cpp
    ${WOWEE_SRC}/pipeline/wowee_transit_schedule.cpp
    ${WOWEE_SRC}/pipeline/wowee_mage_portals.cpp
    ${WOWEE_SRC}/pipeline/wowee_combat_stats.cpp
    ${WOWEE_SRC}/pipeline/wowee_guild_bank.cpp
    ${WOWEE_SRC}/pipeline/wowee_quest_graph.cpp
    ${WOWEE_SRC}/pipeline/wowee_crafting_recipes.cpp
    ${WOWEE_SRC}/pipeline/wowee_world_locations.cpp
    ${WOWEE_SRC}/pipeline/wowee_soulbind_rules.cpp
    ${WOWEE_SRC}/pipeline/wowee_creature_behavior.cpp
    ${WOWEE_SRC}/pipeline/wowee_random_property.cpp
    ${WOWEE_SRC}/pipeline/wowee_spell_proc_rules.cpp
    ${WOWEE_SRC}/pipeline/wowee_auction_houses.cpp
    ${WOWEE_SRC}/pipeline/wowee_battleground_rewards.cpp
    ${WOWEE_SRC}/pipeline/wowee_sound_swap.cpp
    ${WOWEE_SRC}/pipeline/wowee_tutorial_steps.cpp
    ${WOWEE_SRC}/pipeline/wowee_chat_commands.cpp
    ${WOWEE_SRC}/pipeline/wowee_camera_presets.cpp
    ${WOWEE_SRC}/pipeline/wowee_combat_formulas.cpp
    ${WOWEE_SRC}/pipeline/wowee_chat_links.cpp
    ${WOWEE_SRC}/pipeline/custom_zone_discovery.cpp
    ${WOWEE_SRC}/pipeline/dbc_layout.cpp
    ${WOWEE_SRC}/pipeline/terrain_mesh.cpp

    # Rendering (Vulkan infrastructure)
    ${WOWEE_SRC}/rendering/vk_context.cpp
    ${WOWEE_SRC}/rendering/vk_utils.cpp
    ${WOWEE_SRC}/rendering/vk_shader.cpp
    ${WOWEE_SRC}/rendering/vk_texture.cpp
    ${WOWEE_SRC}/rendering/vk_buffer.cpp
    ${WOWEE_SRC}/rendering/vk_pipeline.cpp
    ${WOWEE_SRC}/rendering/vk_render_target.cpp

    # Rendering (scene)
    ${WOWEE_SRC}/rendering/renderer.cpp
    ${WOWEE_SRC}/rendering/camera.cpp
    ${WOWEE_SRC}/rendering/camera_controller.cpp
    ${WOWEE_SRC}/rendering/material.cpp
    ${WOWEE_SRC}/rendering/terrain_renderer.cpp
    ${WOWEE_SRC}/rendering/terrain_manager.cpp
    ${WOWEE_SRC}/rendering/frustum.cpp
    ${WOWEE_SRC}/rendering/performance_hud.cpp
    ${WOWEE_SRC}/rendering/water_renderer.cpp
    ${WOWEE_SRC}/rendering/skybox.cpp
    ${WOWEE_SRC}/rendering/celestial.cpp
    ${WOWEE_SRC}/rendering/starfield.cpp
    ${WOWEE_SRC}/rendering/clouds.cpp
    ${WOWEE_SRC}/rendering/lens_flare.cpp
    ${WOWEE_SRC}/rendering/weather.cpp
    ${WOWEE_SRC}/rendering/lightning.cpp
    ${WOWEE_SRC}/rendering/lighting_manager.cpp
    ${WOWEE_SRC}/rendering/zone_ambience.cpp
    ${WOWEE_SRC}/rendering/sky_system.cpp
    ${WOWEE_SRC}/rendering/character_renderer.cpp
    ${WOWEE_SRC}/rendering/character_preview.cpp
    ${WOWEE_SRC}/rendering/wmo_renderer.cpp
    ${WOWEE_SRC}/rendering/m2_renderer.cpp
    ${WOWEE_SRC}/rendering/m2_renderer_render.cpp
    ${WOWEE_SRC}/rendering/m2_renderer_particles.cpp
    ${WOWEE_SRC}/rendering/m2_renderer_instance.cpp
    ${WOWEE_SRC}/rendering/m2_model_classifier.cpp
    ${WOWEE_SRC}/rendering/render_graph.cpp
    ${WOWEE_SRC}/rendering/hiz_system.cpp
    ${WOWEE_SRC}/rendering/quest_marker_renderer.cpp
    ${WOWEE_SRC}/rendering/footprint_renderer.cpp
    ${WOWEE_SRC}/rendering/minimap.cpp
    ${WOWEE_SRC}/rendering/world_map/coordinate_projection.cpp
    ${WOWEE_SRC}/rendering/world_map/map_resolver.cpp
    ${WOWEE_SRC}/rendering/world_map/exploration_state.cpp
    ${WOWEE_SRC}/rendering/world_map/data_repository.cpp
    ${WOWEE_SRC}/rendering/world_map/view_state_machine.cpp
    ${WOWEE_SRC}/rendering/world_map/composite_renderer.cpp
    ${WOWEE_SRC}/rendering/world_map/overlay_renderer.cpp
    ${WOWEE_SRC}/rendering/world_map/input_handler.cpp
    ${WOWEE_SRC}/rendering/world_map/world_map_facade.cpp
    ${WOWEE_SRC}/rendering/world_map/zone_metadata.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/player_marker_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/party_dot_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/taxi_node_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/poi_marker_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/quest_poi_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/corpse_marker_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/rare_tracker_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/chest_tracker_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/zone_highlight_layer.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/coordinate_display.cpp
    ${WOWEE_SRC}/rendering/world_map/layers/subzone_tooltip_layer.cpp
    ${WOWEE_SRC}/rendering/swim_effects.cpp
    ${WOWEE_SRC}/rendering/mount_dust.cpp
    ${WOWEE_SRC}/rendering/levelup_effect.cpp
    ${WOWEE_SRC}/rendering/charge_effect.cpp
    ${WOWEE_SRC}/rendering/spell_visual_system.cpp
    ${WOWEE_SRC}/rendering/post_process_pipeline.cpp
    ${WOWEE_SRC}/rendering/overlay_system.cpp

    # Rendering (animation)
    ${WOWEE_SRC}/rendering/animation_controller.cpp
    ${WOWEE_SRC}/rendering/animation/animation_ids.cpp
    ${WOWEE_SRC}/rendering/animation/emote_registry.cpp
    ${WOWEE_SRC}/rendering/animation/footstep_driver.cpp
    ${WOWEE_SRC}/rendering/animation/sfx_state_driver.cpp
    ${WOWEE_SRC}/rendering/animation/anim_capability_probe.cpp
    ${WOWEE_SRC}/rendering/animation/locomotion_fsm.cpp
    ${WOWEE_SRC}/rendering/animation/combat_fsm.cpp
    ${WOWEE_SRC}/rendering/animation/activity_fsm.cpp
    ${WOWEE_SRC}/rendering/animation/mount_fsm.cpp
    ${WOWEE_SRC}/rendering/animation/character_animator.cpp
    ${WOWEE_SRC}/rendering/animation/animation_manager.cpp
    ${WOWEE_SRC}/rendering/loading_screen.cpp

    # Addons
    ${WOWEE_SRC}/addons/addon_manager.cpp
    ${WOWEE_SRC}/addons/lua_engine.cpp
    ${WOWEE_SRC}/addons/lua_unit_api.cpp
    ${WOWEE_SRC}/addons/lua_spell_api.cpp
    ${WOWEE_SRC}/addons/lua_inventory_api.cpp
    ${WOWEE_SRC}/addons/lua_quest_api.cpp
    ${WOWEE_SRC}/addons/lua_social_api.cpp
    ${WOWEE_SRC}/addons/lua_system_api.cpp
    ${WOWEE_SRC}/addons/lua_action_api.cpp
    ${WOWEE_SRC}/addons/toc_parser.cpp

    # UI
    ${WOWEE_SRC}/ui/ui_manager.cpp
    ${WOWEE_SRC}/ui/auth_screen.cpp
    ${WOWEE_SRC}/ui/realm_screen.cpp
    ${WOWEE_SRC}/ui/character_create_screen.cpp
    ${WOWEE_SRC}/ui/character_screen.cpp
    ${WOWEE_SRC}/ui/game_screen.cpp
    ${WOWEE_SRC}/ui/chat_panel.cpp
    ${WOWEE_SRC}/ui/combat_ui.cpp
    ${WOWEE_SRC}/ui/dialog_manager.cpp
    ${WOWEE_SRC}/ui/framexml_emitter.cpp
    ${WOWEE_SRC}/ui/game_screen_frames.cpp
    ${WOWEE_SRC}/ui/game_screen_hud.cpp
    ${WOWEE_SRC}/ui/game_screen_minimap.cpp
    ${WOWEE_SRC}/ui/gm_command_screen.cpp
    ${WOWEE_SRC}/ui/interface_fonts.cpp
    ${WOWEE_SRC}/ui/scene_pick.cpp
    ${WOWEE_SRC}/ui/settings_panel.cpp
    ${WOWEE_SRC}/ui/social_panel.cpp
    ${WOWEE_SRC}/ui/action_bar_panel.cpp
    ${WOWEE_SRC}/ui/toast_manager.cpp
    ${WOWEE_SRC}/ui/ui_raid_icons.cpp
    ${WOWEE_SRC}/ui/widget_renderer.cpp
    ${WOWEE_SRC}/ui/widget_tree.cpp
    ${WOWEE_SRC}/ui/window_manager.cpp
    ${WOWEE_SRC}/ui/xml_parser.cpp
    ${WOWEE_SRC}/ui/crafting_window.cpp
    ${WOWEE_SRC}/ui/bis_gear_data.cpp
    ${WOWEE_SRC}/ui/inventory_screen.cpp
    ${WOWEE_SRC}/ui/quest_log_screen.cpp
    ${WOWEE_SRC}/ui/spellbook_screen.cpp
    ${WOWEE_SRC}/ui/talent_screen.cpp
    ${WOWEE_SRC}/ui/keybinding_manager.cpp
    ${WOWEE_SRC}/ui/chat/chat_command_registry.cpp
    ${WOWEE_SRC}/ui/chat/chat_input.cpp
    ${WOWEE_SRC}/ui/chat/chat_markup_parser.cpp
    ${WOWEE_SRC}/ui/chat/chat_markup_renderer.cpp
    ${WOWEE_SRC}/ui/chat/chat_tab_completer.cpp
    ${WOWEE_SRC}/ui/chat/chat_tab_manager.cpp
    ${WOWEE_SRC}/ui/chat/chat_bubble_manager.cpp
    ${WOWEE_SRC}/ui/chat/chat_settings.cpp
    ${WOWEE_SRC}/ui/chat/chat_utils.cpp
    ${WOWEE_SRC}/ui/chat/game_state_adapter.cpp
    ${WOWEE_SRC}/ui/chat/input_modifier_adapter.cpp
    ${WOWEE_SRC}/ui/chat/item_tooltip_renderer.cpp
    ${WOWEE_SRC}/ui/chat/macro_evaluator.cpp
    ${WOWEE_SRC}/ui/chat/macro_eval_convenience.cpp
    ${WOWEE_SRC}/ui/chat/commands/channel_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/combat_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/emote_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/gm_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/group_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/guild_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/help_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/misc_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/social_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/system_commands.cpp
    ${WOWEE_SRC}/ui/chat/commands/target_commands.cpp

    # Android entry point + touch input
    ${WOWEE_ANDROID_SRC}/android_main.cpp
    ${WOWEE_ANDROID_SRC}/touch_input.cpp
)

# --- Excluded on Android ---
# - src/core/macos_platform.mm (Objective-C++, macOS only)
# - src/main.cpp (desktop entry point with X11/signal handlers)
# - src/game/warden_*.cpp (Warden x86 emulation — needs Unicorn, won't work on ARM)
# - src/rendering/amd_fsr3_runtime.cpp (desktop FSR3, not available on mobile)
# - Original audio files (use android_audio_stub.cpp)

# --- Create shared library ---
# Generate version header (same as desktop)
set(WOWEE_VERSION_HEADER ${CMAKE_BINARY_DIR}/generated/core/version.hpp)
execute_process(COMMAND ${CMAKE_COMMAND}
    -DSRC_DIR=${WOWEE_ROOT}
    -DIN_FILE=${WOWEE_ROOT}/include/core/version.hpp.in
    -DOUT_FILE=${WOWEE_VERSION_HEADER}
    -P ${WOWEE_ROOT}/cmake/GitVersion.cmake)
# OpenSSL-dependent auth files: added via list(APPEND) because CMake
# if() blocks don't work inside set() multiline values.
if(NOT WOWEE_OPENSSL_STUBBED)
    list(APPEND WOWEE_WORLD_SOURCES
        ${WOWEE_SRC}/auth/auth_handler.cpp
        ${WOWEE_SRC}/auth/srp.cpp
        ${WOWEE_SRC}/auth/big_num.cpp
        ${WOWEE_SRC}/auth/crypto.cpp
    )
endif()

add_library(wowee SHARED ${WOWEE_WORLD_SOURCES})

# OpenSSL include (prebuilt or stub)
target_include_directories(wowee BEFORE PRIVATE ${WOWEE_OPENSSL_INCLUDE})

# OpenSSL link (prebuilt only)
if(WOWEE_HAS_OPENSSL)
    target_link_libraries(wowee PRIVATE ${WOWEE_SSL_LIB} ${WOWEE_CRYPTO_LIB})
endif()

# Include directories
target_include_directories(wowee PRIVATE
    ${WOWEE_INCLUDE}
    ${WOWEE_SRC}
    ${WOWEE_ANDROID_SRC}
    ${CMAKE_BINARY_DIR}/generated     # version.hpp
)

target_include_directories(wowee SYSTEM PRIVATE
    ${WOWEE_EXTERN}
    ${VK_BOOTSTRAP_DIR}/src
)

# BoringSSL not available in NDK r27 sysroot for linking.
# OpenSSL is stubbed — auth code excluded. Skip ssl/crypto link.
# TODO: link prebuilt OpenSSL for Android when auth is functional.

# Link libraries
target_link_libraries(wowee PRIVATE
    SDL2-static
    Vulkan::Vulkan
    lua51
    imgui
    vk-bootstrap
    Threads::Threads
    ZLIB::ZLIB
    ${CMAKE_DL_LIBS}
    android
    log
    ${SSL_LIB}
    ${CRYPTO_LIB}
)

# Link GLM (header-only)
if(glm_FOUND)
    target_link_libraries(wowee PRIVATE glm::glm)
elseif(EXISTS ${GLM_DIR})
    target_include_directories(wowee SYSTEM PRIVATE ${GLM_DIR})
endif()

# Allow undefined symbols (audio/auth stubbed, NDK toolchain adds --no-undefined)
target_link_options(wowee PRIVATE -Wl,--allow-shlib-undefined)

# Compile options
target_compile_options(wowee PRIVATE
    -O3
    -fvisibility=hidden
    -fvisibility-inlines-hidden
    -ffast-math
    -frtti
    $<$<CONFIG:Debug>:-O0 -g>
)

# Platform-specific compile definitions for Android
target_compile_definitions(wowee PRIVATE
    WOWEE_ANDROID=1
    HAVE_FFMPEG=0
    HAVE_UNICORN=0
    WOWEE_HAS_AMD_FSR2=0
    WOWEE_HAS_AMD_FSR3_FRAMEGEN=0
    WOWEE_AMD_FFX_SDK_KITS=0
    $<$<CONFIG:Release>:WOWEE_RELEASE_LOGGING>
)