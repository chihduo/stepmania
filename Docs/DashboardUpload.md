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
   transfer only new or changed files. The first large import can take several
   minutes; write requests are paced to respect GitHub limits.

Settings are read when the game starts. Moving the portable game to another PC
requires setting the credential again. An expired or revoked token can be
replaced using **Set GitHub Token**. To remove it, delete the corresponding
Generic Credential in Windows Credential Manager.

## If menu actions appear to do nothing

Saving a token is only the authentication step. It does not enable uploading or
set the repository/profile. Close the game, complete the Preferences.ini settings
above, and restart it. **DashboardRepository** must be `owner/repository`, for
example `chihduo/stepmania-dashboard`, without a URL prefix. The two browser
options need this setting even when uploading is disabled.

The options use the title menu’s bold font and selected/unselected sizes. A large
status headline has a shadow and distinct colors: blue for progress, green for
success, amber for warnings, and rose for errors in Chiikawa. Explanations wrap
beneath the headline without covering the menu.

The upload screen displays setup errors and retains the result of token entry.
**Upload / Retry** reports whether an upload is already running or could not
start. Attempting to change the token during an upload, or choosing another
option while returning to the title menu, shows a warning in the status area.
Browser actions report missing repository settings or a failure to launch
the browser, with the destination address for manual use. If a browser opens
behind a full-screen game, switch to it with **Alt+Tab**.

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

If the log reports fewer uploaded play records, also check
`skipped_other_profiles` in `Save/Dashboard/pending.json`. Local-profile uploads
exclude records with a different **or empty** PlayerGuid. Older saves may contain
scores without this field even though their totals were copied into the selected
profile. Check the PlayerGuid in an older `Save/Upload/*.xml` before assuming those
files are missing. Retain the originals; changing to the machine profile does not
resolve the existing dashboard's profile-identity requirement.

For a confirmed sole-player migration from the machine profile, the older logs'
empty PlayerGuid fields can be assigned to the named profile after backing up
the originals. Use the profile's `GeneralData/Guid` from its Stats.xml; this is
not its folder name. Preserve each play's separate `HighScore/Guid`, StageGuid,
timestamp and score fields. All event GUIDs must stay unique. Corrected logs
work with the existing uploader; no additional preference is needed. Merge
replacement files into `Save/Upload`, keep newer logs, then select Upload / Retry.

Use **Open Dashboard** to view the result, or **Open Build Log** to investigate a
publication failure. These buttons use the standard GitHub Pages repository URL.
A GitHub rate-limit or permission error leaves the local data intact; resolve the
error and select Retry later. Disabling `DashboardEnabled` stops this feature's
uploads without changing score recording.
