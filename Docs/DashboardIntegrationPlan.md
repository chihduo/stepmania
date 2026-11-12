# Manual dashboard upload implementation plan

## Agreed behavior

Uploads start only from **Upload Statistics** on the title menu. Windows x64 is the initial integrated-upload platform. Python calculations run in GitHub Actions; GitHub Pages serves generated files. Connection settings live in Preferences.ini, with a Windows Credential Manager reference instead of a plaintext token. Explicitly select one profile. Upload only dashboard statistics, metadata and small banner copies. Store data separately from code on `dashboard-data`; preserve existing history and per-play identities.

## Checkpoints

1. Record this plan and progress before implementation.
2. Define a versioned, allowlisted export; implement dashboard ingestion, duplicate handling, migration checks, tests and a Pages workflow.
3. Add game preferences, a persistent local upload snapshot, native Windows HTTPS/credential support and resumable batch publication. The main thread owns live profile access; workers own immutable export files and network operations.
4. Add a title-menu upload/status screen with progress, cancellation, retry, setup guidance and distinct accepted/published states. Develop Chiikawa-specific presentation on its feature branch and promote only completed runtime files.
5. Test the complete export/import contract, historic-data preservation, duplicate/retry/offline/auth failures and banner persistence. Build Windows x64 and document installation/setup. Commit completed checkpoints.

## Data and publication rules

- Preserve existing Upload score GUIDs; legacy identities are deterministic. Cumulative profile totals replace prior snapshots and are never added repeatedly.
- Filter uploads to the selected profile. Score recording, rate changes and replay eligibility keep their existing semantics.
- Remote commits update the data branch without force-pushing. A separate repository_dispatch triggers trusted code on the default branch; retry dispatch independently after an accepted upload.
- Validate uploads before building. Compare the first generated site against the existing published baseline; incomplete history must not replace the live site.
- Store banner copies by content hash, reuse unchanged copies and retain historical banners.
- Retain an outbox and report accepted versus published separately. No startup, per-song or session-end network activity.
- Do not switch the live Pages source until a complete baseline passes migration checks. Any unavailable user data or Windows credential setup is reported explicitly.

## Validation

Use importer fixtures for malformed/duplicate/wrong-profile input, repeated imports, cumulative totals, missing historical records and banners. Exercise engine export and uploader state transitions with focused tests and a native UI smoke check. Run the Windows workflow for compilation, package checks and executable startup. Publishing personal source history requires the user's matching local Save data; never substitute test plays for that history.
