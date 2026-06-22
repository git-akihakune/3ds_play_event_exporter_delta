# Universal Updater Publishing Notes

Universal Updater uses Universal-DB as its default UniStore. To publish this fork there, submit a pull request to `Universal-Team/db` after the `main` release workflow has produced a tested GitHub Release.

## Pre-submission Checklist

1. Merge this branch to `main` and wait for the `Build and release` workflow to publish a GitHub Release.
2. Download the release asset `play_event_exporter_delta.3dsx` from that release.
3. Test that exact release artifact on real 3DS hardware:
   - The Homebrew Launcher entry is named `PlayEvent Exporter Delta`.
   - The author line is `(c) 2025 TuxSH - Aki Hakune`.
   - The icon is distinct from the original build.
   - Running the app does not crash.
   - `sdmc:/play_events.log` includes resolved app and known applet names such as `HOME Menu` and `Internet Browser`.
   - Remaining hexadecimal title IDs are only for titles that are not installed, not readable through SMDH, and not in the built-in known-system-title table.
4. Make sure the public GitHub repo has a useful description and the release artifact is not a draft or prerelease.

## Universal-DB Entry

Create `source/apps/play-event-exporter-delta.json` in a fork of `Universal-Team/db` and open a pull request. Replace `OWNER` with the final GitHub owner or organization:

```json
{
	"github": "OWNER/3ds_play_event_exporter",
	"title": "PlayEvent Exporter Delta",
	"systems": [
		"3DS"
	],
	"categories": [
		"utility"
	],
	"description": "Exports 3DS Play History telemetry to sdmc:/play_events.log with title names when available.",
	"download_filter": "\\.3dsx$",
	"icon": "https://raw.githubusercontent.com/OWNER/3ds_play_event_exporter/main/resources/icon.png",
	"llm_generation": "yes"
}
```

Notes:

- `github` lets Universal-DB pull metadata and release assets from GitHub Releases.
- `download_filter` is a regular expression matched against release asset names. Use `\.3dsx$` for the raw Homebrew Launcher build. The release ZIP is useful for manual downloads, but Universal-DB does not auto-generate install scripts for archives unless the entry defines an `archive` mapping.
- Omit `unique_ids` for now because this project publishes a `.3dsx` homebrew launcher app, not a CIA title with a stable title ID.
- Keep `llm_generation` honest. This branch was developed with LLM assistance and includes an LLM-assisted icon asset, so `yes` is the conservative declaration.

## References

- Universal-Updater README: <https://github.com/Universal-Team/Universal-Updater>
- Universal-DB contribution rules: <https://github.com/Universal-Team/db/blob/master/CONTRIBUTING.md>
- Universal-DB app entries: <https://github.com/Universal-Team/db/tree/master/source/apps>
