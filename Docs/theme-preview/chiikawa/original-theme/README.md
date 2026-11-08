# Original theme components used by the preview

These PNG and INI files are exact copies of assets already in this StepMania repository. They preserve the original glyphs, arrow/receptor shapes, and judgment lettering. Their original paths and SHA-256 hashes are listed in [sources.json](sources.json). Existing [license notices](../../../../Docs/Licenses.txt) continue to apply.

| Preview role | Original theme mapping |
| --- | --- |
| Body and menu text | `Common Normal` → Roboto Medium 24px |
| Headings | `Common Header` → Open Sans Semibold 48px |
| Compact labels | `Common Condensed` → Roboto Condensed 24px |
| Speed / combo label | Roboto Condensed Bold Italic 24px |
| Combo numbers | `Combo Numbers` → Roboto Bold 54px |
| Score digits | `ScoreDisplayNormal Text` → Overpass Score |
| Tap notes and receptors | Bundled dance/default sprite textures, rotated using its direction mapping |
| Gameplay judgments | Default theme `Judgment Normal 2x6.png`, including its original shading and lettering |

The browser draws the bitmap glyphs using their original atlas pages and advance widths. It changes text color through the existing glyph alpha, without inventing letterforms. Note and judgment textures keep their source colors so rhythm subdivisions, judgment meanings, and shading remain distinct. Their cyan/pastel accents already fit parts of the new palette.

This is a static artwork/layout preview. It does not implement the engine's complete font fallback, animation, texture filtering, lighting, or gameplay behavior. The English sample text is covered by these atlas pages; a real inherited theme retains the engine's international glyph fallback. Original assets are used directly, not passed through the image generator. The AI-generated illustrations are in the separate `images/` directory.
