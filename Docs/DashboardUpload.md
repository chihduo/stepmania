# Upload play statistics to your dashboard

Choose **Upload Statistics** on the title menu to update your GitHub Pages
dashboard. Uploads happen only when you select this option. Once GitHub accepts
the upload and starts its workflow, you can close the game; publication continues
online. The screen distinguishes an accepted upload from a published dashboard.

Integrated uploading currently supports **Windows x64**. Python runs on GitHub's
servers. The Windows game does not need Python, Git, WSL or the GitHub CLI.
Linux builds can prepare the same export for local inspection.

## First-time setup

Your dashboard repository needs the new importer and publication workflow. See
[the dashboard setup guide](https://github.com/chihduo/stepmania-dashboard/blob/main/SYNC.md).

1. Close StepMania and edit `Save/Preferences.ini`. Add these entries to its
   existing `[Options]` section, replacing the repository and profile as needed:

   ```ini
   DashboardEnabled=1
   DashboardRepository=chihduo/stepmania-dashboard
   DashboardDataBranch=dashboard-data
   DashboardProfile=local:00000000
   DashboardCredentialKey=StepManiaDashboard
   DisableUploadDir=0
   ```

   Use the directory name under `Save/LocalProfiles/` for the local profile ID.
   For example, `Save/LocalProfiles/00000001/` means `local:00000001`. The upload
   screen lists available profiles if the configured one is missing. Use
   `DashboardProfile=machine` only for a dashboard intentionally based on the
   machine profile. Your existing dashboard uses a local profile.

2. Create a **fine-grained personal access token** in GitHub Settings → Developer
   settings → Personal access tokens. Select only your dashboard repository and
   grant **Contents: read and write**. Set an expiration you can manage.
   Do not paste the token into Preferences.ini or send it in chat.
3. Restart the game, choose **Upload Statistics**, then **Set GitHub Token**.
   Paste the token with **Ctrl+V** into the masked input and press Enter. It is stored under `DashboardCredentialKey`
   in Windows Credential Manager, associated with your Windows user account.
4. Select **Upload / Retry**. The first upload imports the available historical
   profile, per-play logs, song details and small banner copies. Later uploads
   transfer only new or changed files.

Settings are read when the game starts. Moving the portable game to another PC
requires setting the credential again. An expired or revoked token can be
replaced using **Set GitHub Token**. To remove it, delete the corresponding
Generic Credential in Windows Credential Manager.

## What gets uploaded

The selected profile's cumulative totals and score lists come from its current
in-memory statistics. Individual plays come from `Save/Upload`, filtered to that
profile. Live playback-speed changes count normally. Watching a replay does not
create another play. Existing scoring behavior is unchanged.

The export includes song titles/artists, difficulty and chart radar information,
and banner thumbnails up to 160 pixels on their longest side. Banners use cached
images when available, with ordinary image files as a fallback. Movie banners
without a cached still use the dashboard placeholder or a previously published
thumbnail; song movies and audio are never uploaded.

The data branch stores statistics under `profiles/<guid>/` and content-addressed
banner envelopes under `banners/`. GitHub Actions verifies and converts the banner
envelopes into ordinary PNGs at the website's `/banners/` URLs. A repository's data
branch has the same public/private visibility as that repository. For this public
dashboard, uploaded score history and thumbnails are public. Configuration,
credentials, full note charts, replay files, song media and unrelated profile
fields are excluded by the exporter.

## Retries and history protection

The game keeps its local export in `Save/Dashboard/outbox/` and records the last
accepted upload in `Save/Dashboard/last-upload.json`. Keep these with the rest of
Save when upgrading. Original files in `Save/Upload` are never deleted.

Retries preserve play IDs, reuse unchanged files and update the data branch only
when a complete batch is ready. A cancelled or interrupted upload may already
have been accepted by GitHub; Retry checks the remote state before adding data.
If publication fails after upload, Retry can start it again without duplicating
plays. Returning to the menu stops local work/status polling; an already started
GitHub workflow can continue. There is no automatic resume on startup.

Before publishing, Actions compares the new dashboard with the current live site.
A different profile, reduced lifetime/song totals, or missing daily/monthly play
history fails validation and leaves the existing website intact. Restore the
matching `Save/LocalProfiles/<id>` and `Save/Upload` backup, then retry. Keeping just
Stats.xml preserves aggregate scores but cannot recover a deleted per-play log.

Use **Open Dashboard** to view the result, or **Open Build Log** to investigate a
publication failure. These buttons use the standard GitHub Pages repository URL.
A GitHub rate-limit or permission error leaves the local data intact; resolve the
error and select Retry later. Disabling `DashboardEnabled` stops this feature's
uploads without changing score recording.
