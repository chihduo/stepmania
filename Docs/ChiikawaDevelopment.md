# Chiikawa development and release branches

**`main` is this repository's main branch.** Develop the Chiikawa theme on **`feature/chiikawa-theme`** and promote completed revisions as focused release commits.

| Main branch: `main` | Development branch: `feature/chiikawa-theme` |
| --- | --- |
| Playable `Themes/chiikawa/`, including the final silent MP4 and poster | The same runtime theme under development |
| Required background/replay hooks and regression tests | Experimental implementation and checkpoint commits |
| User guides, native screenshots, illustration pools and release asset manifest | Browser gallery, alternative artwork, prompts and full provenance records |
| Concise release results in the main plan/progress files | Original video clips, assembly recipe and detailed theme plans/progress |

## Open the development workspace

The separate worktree created for this split is `../stepmania-chiikawa` beside the main checkout. It is already on `feature/chiikawa-theme`; edit there for future theme work. Each worktree has its own files and shares Git objects and branches.

In another checkout, fetch the published branch and create a worktree with:

```sh
git fetch fork
git worktree add ../stepmania-chiikawa feature/chiikawa-theme
```

If the local branch does not exist yet, use `git worktree add -b feature/chiikawa-theme ../stepmania-chiikawa fork/feature/chiikawa-theme` instead. The `fork` remote is this repository's `chihduo/stepmania` remote.

The feature branch retains `Docs/theme-preview/chiikawa/index.html` for previews, `dance-movie/` for the original clips and assembly recipe, and `DEVELOPMENT_PLAN.md` / `DEVELOPMENT_PROGRESS.md` in the preview folder for detailed theme work. Earlier development commits remain available in its history. Build and verification output stays ignored under `out/`.

## Promote a completed revision

1. In the feature worktree, commit the completed work, merge the latest `main`, resolve any conflicts, and run the relevant checks. Commit any resolutions. This ensures shared engine changes are current before copying files back.
2. From a clean main worktree, create an integration branch and copy the reviewed runtime files and tests. The example below covers the existing theme and fallback effect; review any other required engine changes separately.

```sh
git switch main
git switch -c integrate/chiikawa-release
git restore --source=feature/chiikawa-theme -- \
  Themes/chiikawa \
  Data/Defaults.ini \
  Themes/default/ThemeInfo.ini \
  Themes/default/readme.md \
  BackgroundEffects/StretchNormal.lua \
  src/tests/test_chiikawa_theme.lua \
  src/tests/test_fallback_movie.lua
```

3. Include the shipped `Data/Defaults.ini`, Classic display label in `Themes/default/ThemeInfo.ini`, and required-path checks in `ci/Test-WindowsPortable.ps1` when promoting the default-theme revision. Keep the original `Themes/default` directory for inheritance and saved choices. Update user-facing docs and the release manifest, copy finished native screenshots as needed, and review the diff. Keep the comparison gallery, raw inputs and development journals on the feature branch. The established main documentation paths are `README.md`, `Themes/chiikawa/README.md`, `Themes/chiikawa/Art/README.md`, this guide, and `Docs/theme-preview/chiikawa/{README.md,artwork-pools.json,release-manifest.json,native/}`. Preview-folder docs on the feature branch include development material, so copy only the intended release files.
4. Check asset hashes and local document links, run the focused Lua checks with the bundled Lua interpreter, and perform native verification when behavior changes. Commit the finished snapshot. Advance `main` by fast-forwarding this integration branch, then delete the temporary integration branch.
5. Merge `main` back into the feature worktree before starting the next round. The initial split reconciles that baseline while retaining the feature-only files, so subsequent normal merges preserve them unless main explicitly changes those paths.

Promote selected files rather than merging or squash-merging the entire feature tree, which would also bring the development assets into main. Shared engine fixes can instead be committed separately and cherry-picked after review.

## Initial split and history

The main branch was named `5_1-new` at the initial split; its published tip was `0e53193abf`. It now uses the name `main`, with history preserved. The six unpublished movie commits through `a74362c422` are retained on the feature branch; main receives their tested runtime result in one release commit, together with the documentation separation. The final runtime files and regression tests match that tested movie revision byte-for-byte.

Previously published experimental files are removed from the main branch's current tree. Their older Git objects remain in history; this separation does not shrink historical clones or require a force-push. All original artwork and uploaded movies remain available on the feature branch.

When publishing completed theme work, push `feature/chiikawa-theme` first and then `main`, using ordinary pushes. No history rewrite or force-push is needed. This workflow does not build or publish a Windows ZIP.
