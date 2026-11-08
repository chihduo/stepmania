# Animated Chiikawa menu backgrounds

**Yes: the existing engine already supports this through theme Lua and graphics. No recompilation is required for the background effect.** The preview adapts the original title's rotating layers and moving particles to the warm pastel palette.

## Existing implementation

| Original file | What it provides |
| --- | --- |
| [Title background](../../../Themes/default/BGAnimations/ScreenTitleMenu%20background/default.lua) | `FancyUIBG` switch, layered textures, `spin`, opacity, and rotation speed |
| [Title particles](../../../Themes/default/BGAnimations/ScreenTitleMenu%20background/_particleLoader.lua) | Per-frame particle movement and wrapping at screen edges |
| [Shared menu background](../../../Themes/default/BGAnimations/ScreenWithMenuElements%20background/default.lua) | Rotating layers plus scrolling texture accents for other menus |
| [Theme preferences](../../../Themes/default/Scripts/02%20ThemePrefs.lua) | Existing `FancyUIBG` preference |

The option is presented as **Fancy UI Background** in the default theme. Original title layers rotate in opposite directions at different speeds; the particle loader moves 13 particles across the screen. These operations are available to an inherited Chiikawa theme.

## What the preview demonstrates

The animated character title uses a cream base, three pink/mint/blue layers, nine drifting particles, then the selected transparent character illustration and original bitmap UI text. The layer shapes and particles are exact copies of the original title textures; the browser tints their alpha without changing the source PNGs. The three layers complete a rotation in 80, 48, and 65 seconds. Particle drift is slow and continuous. The characters themselves remain still.

Song selection, evaluation, and replay use the same treatment behind panels and character cutouts. Gameplay receives no animated decorative layer. Opaque painted title alternatives and the original painted selection scene retain their static backgrounds so artwork does not need to be artificially separated into layers.

Use **Background motion** to pause/resume. The browser honors `prefers-reduced-motion` by starting paused; an explicit user choice overrides that starting preference. Hidden screens pause their motion. The local preview works from `file://` without a server or remote assets.

The effect lives in [menu-motion.js](menu-motion.js) and the gallery's CSS. Browser motion, tint, and layout are a design demonstration rather than a claim of identical engine rendering.

## Native theme implementation

[Themes/chiikawa](../../../Themes/chiikawa/README.md) now implements cream menu backgrounds with three slow rotating original texture layers and nine gently drifting particles. Character artwork is a separate screen underlay, so it is selected on actual entry even when the shared background is cached. Pushed replay-browser screens own their backdrop because they do not activate a new shared background.

The existing **Fancy UI Background** option controls native motion (`FancyUIBG` in `Save/ThemePrefs.ini`, `[chiikawa]`). Disabling it leaves the cream background and static characters. Gameplay keeps its song background/video and static character decoration. The native engine multiplies texture colors while the browser tints their alpha, so the animation is similar rather than pixel-identical.

The animation needs only Lua and PNG files. Optional replay-browser styling commands are a separate small engine addition. Actual Linux rendering is checked at 16:9 and 4:3; Windows/audio/video validation remains a manual check on the next Windows build.

See [artwork selection](ARTWORK_CHOICES.md) for the independent random-per-screen policy. The browser's reduced-motion behavior is separate from the native Fancy UI Background setting.
