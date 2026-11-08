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

The animated duet title uses a cream base, three pink/mint/blue layers, nine drifting particles, then the transparent two-character illustration and original bitmap UI text. The layer shapes and particles are exact copies of the original title textures; the browser tints their alpha without changing the source PNGs. The three layers complete a rotation in 80, 48, and 65 seconds. Particle drift is slow and continuous. The characters themselves remain still.

Song selection, evaluation, and replay use the same treatment behind panels and character cutouts. Gameplay receives no animated decorative layer. Opaque painted title alternatives and the original painted selection scene retain their static backgrounds so artwork does not need to be artificially separated into layers.

Use **Background motion** to pause/resume. The browser honors `prefers-reduced-motion` by starting paused; an explicit user choice overrides that starting preference. Hidden screens pause their motion. The local preview works from `file://` without a server or remote assets.

The effect lives in [menu-motion.js](menu-motion.js) and the gallery's CSS. Browser motion, tint, and layout are a design demonstration rather than a claim of identical engine rendering.

## Native theme integration

1. Create the separate inherited theme described in [ARTWORK_PLAN.md](ARTWORK_PLAN.md). Keep the existing font, noteskin, and judgment mappings.
2. Add a shared Lua background actor: cream base first, optional rotating textures/particles next, and the selected paired illustration above them. Use normal alpha blending over the pale base rather than the original title's additive blend.
3. Gate animation with `ThemePrefs.Get("FancyUIBG")`; retain a readable still background when disabled. Use existing `spin` / `effectmagnitude` commands for rotation and either actor tweens or an update function for drift. Match the preview speeds and opacity by visual review in the engine.
4. Load it from the title and shared menu background entry points. Keep decorations behind labels and panels. Leave gameplay background/video handling and note readability intact.
5. Verify both aspect ratios in the actual Windows x64 renderer, including theme switching, animation off/on, screen transitions, and slow machines. These native checks remain part of the later theme integration.

Only theme Lua, metrics, and graphics are needed for this animation. Existing replay-widget styling hooks discussed in the broader plan are a separate integration concern.
