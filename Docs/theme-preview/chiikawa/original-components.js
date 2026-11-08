/* Preview original bitmap fonts and sprite frames without changing source images. */
(() => {
  const fonts = window.ORIGINAL_THEME_FONTS;
  const images = {};
  const runs = [];
  const missing = new Set();
  const load = name => new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => { images[name] = img; resolve(); };
    img.onerror = () => reject(new Error('Cannot load original font: ' + name));
    img.src = fonts[name].image;
  });
  function fontFor(element) {
    const explicit = element.closest('[data-font]');
    if (explicit) return explicit.dataset.font;
    if (element.closest('.score,.result-score strong')) return 'score';
    if (element.closest('.combo-number')) return 'combo';
    if (element.closest('.rate,.combo-label')) return 'rate';
    if (element.closest('.title-wordmark,.screen-heading,.song-details h3')) return 'header';
    if (element.closest('.graph-label,.hint,.recording')) return 'condensed';
    return 'normal';
  }
  function replaceText(root) {
    const walker = document.createTreeWalker(root, NodeFilter.SHOW_TEXT);
    const nodes = [];
    while (walker.nextNode()) {
      if (walker.currentNode.nodeValue.trim()) nodes.push(walker.currentNode);
    }
    nodes.forEach(node => {
      const parent = node.parentElement;
      if (parent.closest('canvas,script,style')) return;
      const words = node.nodeValue.replace(/\s+/g, ' ').match(/\S+\s*|\s+/g) || [];
      const font = fontFor(parent);
      const fragment = document.createDocumentFragment();
      words.forEach(text => {
        const canvas = document.createElement('canvas');
        canvas.className = 'theme-glyphs';
        canvas.setAttribute('role', 'img');
        canvas.setAttribute('aria-label', text.trim());
        canvas.dataset.font = font;
        fragment.append(canvas);
        fragment.append(document.createElement('wbr'));
        runs.push({canvas, text, font});
      });
      node.replaceWith(fragment);
    });
  }
  function draw(run) {
    const {canvas, text, font: name} = run;
    if (!canvas.getClientRects().length) return;
    const style = getComputedStyle(canvas.parentElement);
    const f = fonts[name];
    const image = images[name];
    const scale = parseFloat(style.fontSize) / f.nominal;
    const spacing = parseFloat(style.letterSpacing) || 0;
    const chars = Array.from(text);
    const glyphs = chars.map(char => {
      if (!f.glyphs[char]) { missing.add(name + ': ' + char); return f.glyphs['?'] || f.glyphs[' ']; }
      return f.glyphs[char];
    });
    const pad = 4 * scale;
    const width = glyphs.reduce((sum,g) => sum + (g.width + f.advance) * scale + spacing, 0) + pad * 2;
    const height = (f.cellHeight - f.top) * scale;
    const ratio = window.devicePixelRatio || 1;
    canvas.width = Math.max(1, Math.ceil(width * ratio));
    canvas.height = Math.max(1, Math.ceil(height * ratio));
    canvas.style.width = width + 'px';
    canvas.style.height = height + 'px';
    canvas.style.marginLeft = -pad + 'px';
    canvas.style.marginRight = -pad + 'px';
    const ctx = canvas.getContext('2d');
    ctx.scale(ratio, ratio);
    let x = pad;
    glyphs.forEach(g => {
      const col = g.frame % f.columns;
      const row = Math.floor(g.frame / f.columns);
      // Keep the original cell artwork and engine advance width. Odd chops
      // follow FontPage::SetTextureCoords, avoiding half-pixel source shifts.
      let chop = f.cellWidth - g.width;
      if (chop % 2) chop -= 1;
      const sx = col * f.cellWidth * f.density;
      const sy = (row * f.cellHeight + f.top) * f.density;
      ctx.drawImage(image, sx, sy, f.cellWidth * f.density,
        (f.cellHeight - f.top) * f.density,
        x - chop / 2 * scale, 0, f.cellWidth * scale, height);
      x += (g.width + f.advance) * scale + spacing;
    });
    // Only tint the original alpha glyphs; do not redraw or substitute letters.
    ctx.globalCompositeOperation = 'source-in';
    ctx.fillStyle = style.color;
    ctx.fillRect(0, 0, width, height);
  }
  function redraw() {
    if (!Object.keys(fonts).every(key => images[key])) return;
    runs.forEach(draw);
    document.documentElement.dataset.originalFonts = 'ready';
    document.documentElement.dataset.missingGlyphs = [...missing].join(', ');
  }
  window.redrawOriginalComponents = redraw;
  window.addEventListener('resize', redraw);
  document.addEventListener('DOMContentLoaded', () => {
    document.querySelectorAll('.screen,.font-proof').forEach(replaceText);
    Promise.all(Object.keys(fonts).map(load)).then(redraw).catch(error => {
      document.documentElement.dataset.originalFonts = 'error';
      console.error(error);
    });
  });
})();
