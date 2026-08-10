#pragma once

// Generated from git at build time — see cmake/GitVersion.cmake.
// kVersion is the last tag reachable from HEAD, so tagging a release is all it
// takes to update the version the client reports.

namespace wowee {
namespace core {

inline constexpr const char* kVersion = "10351868";
inline constexpr const char* kBuildDate = "2026-08-09";

// "v2.0.3-preview (built 2026-07-12)" — what the login screen and settings show.
inline constexpr const char* kVersionString =
    "10351868 (built 2026-08-09)";

} // namespace core
} // namespace wowee
