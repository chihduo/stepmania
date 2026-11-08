# Working branches

`5_1-new` is the main release branch. Develop Chiikawa artwork, theme behavior, previews and generation assets on `feature/chiikawa-theme`, preferably in the separate `../stepmania-chiikawa` worktree. Read [the branch workflow](Docs/ChiikawaDevelopment.md) before theme work or promotion.

Keep prompts, alternative illustrations, raw movie clips, assembly scripts, the browser gallery and detailed theme journals on the feature branch. Its detailed plan and progress files are `Docs/theme-preview/chiikawa/DEVELOPMENT_PLAN.md` and `DEVELOPMENT_PROGRESS.md`.

Promote only completed runtime theme files, required shared hooks/tests, user documentation, final screenshots and the release manifest to main. Copy reviewed paths or cherry-pick focused release commits; merging the entire feature tree would import development assets. Keep the main plan/progress entries concise and record finished results there.

Other engine work continues on main as usual. Preserve existing published history. Explicit user instructions take precedence over this workflow.
