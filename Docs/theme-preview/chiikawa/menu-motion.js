/* Browser study of the default theme's rotating layers and particles. */
(() => {
  const rings = [
    {size: '90%', tint: '#f7bbc8', opacity: .32, duration: '80s', angle: '-20deg', direction: 'normal'},
    {size: '62%', tint: '#cbe7d7', opacity: .48, duration: '48s', angle: '-10deg', direction: 'reverse'},
    {size: '36%', tint: '#bfdcf0', opacity: .42, duration: '65s', angle: '0deg', direction: 'normal'}
  ];
  const textures = {};
  const loadTexture = name => new Promise((resolve, reject) => {
    const image = new Image();
    image.onload = () => { textures[name] = image; resolve(); };
    image.onerror = () => reject(new Error('Cannot load menu texture: ' + name));
    image.src = 'original-theme/menu-' + name + '.png';
  });
  const colors = ['#e59eb2', '#9cc9b3', '#99c1dd'];
  const media = matchMedia('(prefers-reduced-motion: reduce)');
  let userSelected = false;
  function setMotion(enabled) {
    document.getElementById('stage').dataset.motion = enabled ? 'on' : 'off';
    const button = document.getElementById('motion-toggle');
    button.setAttribute('aria-pressed', String(enabled));
    button.textContent = 'Background motion: ' + (enabled ? 'on' : 'off');
    document.getElementById('motion-status').textContent = enabled
      ? 'Slow rotating layers and drifting particles on menu screens. Gameplay stays still.'
      : 'Motion paused. The menu keeps a still pastel background.';
  }
  function part(className, variables) {
    const item = document.createElement('canvas');
    const texture = textures[className === 'motion-ring' ? 'tunnel' : 'particle'];
    item.width = texture.width; item.height = texture.height;
    const context = item.getContext('2d');
    context.drawImage(texture, 0, 0);
    context.globalCompositeOperation = 'source-in';
    context.fillStyle = variables.tint;
    context.fillRect(0, 0, item.width, item.height);
    item.className = className;
    for (const [key, value] of Object.entries(variables)) item.style.setProperty('--' + key, value);
    return item;
  }
  document.addEventListener('DOMContentLoaded', async () => {
    try { await Promise.all(['tunnel', 'particle'].map(loadTexture)); }
    catch (error) { document.documentElement.dataset.menuMotion = 'error'; console.error(error); return; }
    document.querySelectorAll('.screen:not([data-id="game"])').forEach(screen => {
      const layer = document.createElement('div');
      layer.className = 'motion-layer';
      layer.setAttribute('aria-hidden', 'true');
      rings.forEach(ring => layer.append(part('motion-ring', ring)));
      for (let i = 0; i < 9; i++) {
        layer.append(part('motion-particle', {
          x: (9 + i * 11) + '%', y: (12 + (i * 23) % 77) + '%',
          size: (1.5 + i % 3 * .45) + 'cqw', tint: colors[i % colors.length],
          duration: (14 + i * 1.7) + 's', delay: (-i * 2.4) + 's'
        }));
      }
      screen.prepend(layer);
    });
    const requested = new URLSearchParams(location.search).get('motion');
    userSelected = requested === 'on' || requested === 'off';
    setMotion(userSelected ? requested === 'on' : !media.matches);
    document.getElementById('motion-toggle').addEventListener('click', () => {
      userSelected = true;
      setMotion(document.getElementById('stage').dataset.motion !== 'on');
    });
    media.addEventListener('change', () => { if (!userSelected) setMotion(!media.matches); });
    document.documentElement.dataset.menuMotion = 'ready';
  });
})();
