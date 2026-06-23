# PlayEvent Exporter Delta

[![Build and release](https://github.com/git-akihakune/3ds_play_event_exporter/actions/workflows/release.yml/badge.svg?branch=master)](https://github.com/git-akihakune/3ds_play_event_exporter/actions/workflows/release.yml)
[![License: ISC](https://img.shields.io/badge/License-ISC-blue.svg)](LICENSE)
![Platform: Nintendo 3DS](https://img.shields.io/badge/platform-Nintendo%203DS-red)
![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white)

PlayEvent Exporter Delta is a Nintendo 3DS homebrew utility that exports the system's hidden PlayEvent history to `sdmc:/play_events.log`.

The 3DS records more activity than the public Activity Log shows: application and applet launches, HOME Menu jumps, suspends and resumes, shell open/close events, shutdowns, and user clock changes. This tool reads that history directly from PTM, resolves title IDs to readable names where possible, and writes a plain text log you can inspect, archive, or analyze elsewhere.

## Highlights

- Exports the full PTM PlayEvent ring buffer to the SD card.
- Resolves installed titles from SMDH metadata using the console's language when available.
- Includes built-in names for common system applets such as HOME Menu, Internet Browser, Friend List, Notifications, and more.
- Falls back to 16-digit hexadecimal title IDs for unresolved titles, so no events are silently hidden.
- Combines paired user time-change records into readable `from ... to ...` lines.
- Handles the special PTM logging behavior for GBA and DSi software.
- Includes host-side unit tests for the SMDH parser, title-name fallback, and output path.

## Output Example

The exported log is line-oriented and timestamped in the 3DS system clock's local user time:

```log
2015-01-11 04:41: System shutdown
2015-01-11 04:44: Applet launch HOME Menu
2015-01-11 04:45: Shell close
2015-01-11 04:45: Shell open
2015-01-11 04:47: Leave applet HOME Menu
2015-01-11 04:47: Application launch Nintendo 3DS Camera
2015-01-11 04:48: Leave application Nintendo 3DS Camera
2015-01-11 04:48: Jump to applet HOME Menu
2015-01-11 04:48: Application exit Nintendo 3DS Camera
2024-08-31 17:45: User time change from 2015-01-11 04:52 to 2024-08-31 17:45
2025-11-24 17:51: Application launch (0004800049524546)
2025-11-24 17:51: DSi application start
```

When a title name cannot be found, the title ID remains in the log:

```log
2025-11-24 04:37: Application launch (0004001000022100)
```

## Usage

1. Copy `play_event_exporter_delta.3dsx` to your SD card at `sdmc:/3DS`.
2. Launch it from the Homebrew Launcher on a 3DS-family console.
3. Wait for the export message on the top screen. The screen can be black for several minutes while running.
4. Press START to exit.
5. Open `sdmc:/play_events.log` from the SD card.

Each run replaces the previous `play_events.log`.

## Building

The 3DS build uses CMake with the devkitARM 3DS toolchain. The project also supports the `catnip` CMake wrapper used by the release workflow:

```bash
catnip build
```

Release artifacts are named:

- `play_event_exporter_delta.3dsx`
- `play_event_exporter_delta.smdh`
- `play_event_exporter_delta.zip`

## Host-Side Tests

The host tests build without the 3DS toolchain and cover the portable pieces of the exporter:

```bash
cmake -S . -B build/host-tests -G Ninja -DBUILD_HOST_TESTS=ON
cmake --build build/host-tests
ctest --test-dir build/host-tests --output-on-failure
```

## What PTM Records

PlayEvent entries use minute-resolution timestamps counted from `2000-01-01 00:00`. The supported event types are:

| Event | Meaning |
| --- | --- |
| Application launch / exit | A 3DS application started or ended. Invalid title IDs are used for DSi start/exit markers. |
| Applet launch / exit | An applet started or ended. HOME Menu is included in applet events. |
| Jump to application / applet | The foreground context moved into an application or applet. |
| Leave application / applet | The foreground context left an application or applet, such as suspending to HOME Menu. |
| Shell close / open | The shell was closed or opened. These entries do not include title IDs. |
| System shutdown | PTM recorded a shutdown event. |
| User time change | The user clock changed; the exporter prints the old and new times together when both records are present. |

### Shutdown Detection

PTM updates a `System shutdown` entry in the always-on MCU RAM about every five minutes. Unless the battery is removed, that makes unexpected shutdowns visible with useful, but not exact, timing.

### GBA and DSi Titles

GBA and DSi software is logged differently from native 3DS applications. Outside shutdown detection, PTM mainly tracks shell close/open segments while `AgbBg` or `TwlBg` is active.

There can be up to 12 segments starting on a given day, covering up to seven days from that day's start (`D+0 00:00` through `D+6 23:59`). Segment start times have 10-minute resolution, and duration precision depends on segment length; long 12-to-24-hour segments can be as coarse as 20 minutes.

Example GBA sequence:

```log
2025-11-24 17:50: Application launch (0004000000075300)
2025-11-24 17:51: Shell close
2025-11-24 18:00: Shell open
2025-11-24 18:01: Application exit (0004000000075300)
2025-11-24 18:01: System shutdown
2025-11-24 17:51: Applet launch HOME Menu
```

Example DSi sequence:

```log
2025-11-24 17:51: Application launch (0004800049524546)
2025-11-24 17:51: System shutdown
2025-11-24 17:51: DSi application start
2025-11-24 17:52: Shell close
2025-11-24 18:00: Shell open
2025-11-24 18:01: DSi application exit
2025-11-24 17:52: Applet launch HOME Menu
```

## License

Copyright (c) 2026 TuxSH - Aki Hakune.

Licensed under the ISC License. See [LICENSE](LICENSE).
