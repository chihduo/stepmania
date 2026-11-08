/* Artwork is chosen by screen purpose, once on entry, or explicitly by the user. */
const artworkNames = {
  '01-character-trio': 'Character trio',
  '04-results-replay': 'Celebration trio',
  '10-rest': 'Resting pair',
  '11-listening-together': 'Listening together',
  '12-acoustic-duet': 'Acoustic duet',
  '13-shared-jump': 'Shared jump',
  '14-piano-duet': 'Piano duet',
  '15-drum-circle': 'Drum circle',
  '16-violin-serenade': 'Violin serenade',
  '17-flute-practice': 'Flute practice',
  '18-saxophone-swing': 'Saxophone swing'
};
// First entry is the recommended default. Only these compatible cutouts can
// appear in random mode; painted scenes are a separate title-scene choice.
const artworkPools = {
  "title": [
    "12-acoustic-duet",
    "01-character-trio",
    "11-listening-together",
    "14-piano-duet",
    "15-drum-circle",
    "16-violin-serenade",
    "17-flute-practice",
    "18-saxophone-swing"
  ],
  "select": [
    "11-listening-together",
    "12-acoustic-duet",
    "01-character-trio",
    "14-piano-duet",
    "15-drum-circle",
    "16-violin-serenade",
    "17-flute-practice",
    "18-saxophone-swing"
  ],
  "game": [
    "10-rest",
    "11-listening-together",
    "14-piano-duet",
    "16-violin-serenade",
    "17-flute-practice"
  ],
  "result": [
    "04-results-replay",
    "13-shared-jump",
    "15-drum-circle",
    "18-saxophone-swing"
  ],
  "replay": [
    "10-rest",
    "11-listening-together",
    "12-acoustic-duet",
    "14-piano-duet",
    "16-violin-serenade",
    "17-flute-practice"
  ]
};
const screenNames = {title:'Title', select:'Song selection', game:'Gameplay', result:'Evaluation', replay:'Replays'};
const captions = {
  title: 'Title concept: original bitmap text with paired/group artwork and gentle background motion. Painted scene alternatives are static.',
  select: 'Selection concept: original bitmap text, clear focus, chart choice, and room for the selected character decoration.',
  game: 'Gameplay concept: original note/receptor textures, FLAWLESS sprite, score/combo/rate fonts. Decoration stays still outside the note field; the graph is a layout sketch.',
  result: 'Evaluation concept: original bitmap fonts with supportive decoration clear of the result rows. Numbers are illustrative sample data.',
  replay: 'Replay concept: original Common Normal typography with the selected illustration. The current C++ widgets still need theme styling hooks.'
};
const decorationClasses = {title:'.title-friends', select:'.selection-friends', game:'.game-friends', result:'.result-friends', replay:'.replay-friends'};
const modeControl = document.getElementById('artwork-mode');
const decorationControl = document.getElementById('decoration');
const titleControl = document.getElementById('title-art');
let currentScreen = null;

function screenElement(name) { return document.querySelector('.screen[data-id="' + name + '"]'); }
function paintedTitle() { return currentScreen === 'title' && titleControl.value !== 'duet'; }
function applyArtwork(screen, name) {
  const section = screenElement(screen);
  const image = section.querySelector(decorationClasses[screen]);
  image.src = 'images/' + name + '.png';
  image.alt = artworkNames[name] + ' — Chiikawa character decoration';
  section.dataset.artwork = name;
  if (screen === 'select') section.classList.add('with-cutout');
}
function chooseRandom(screen) {
  const previous = screenElement(screen).dataset.artwork;
  const eligible = artworkPools[screen].filter(name => name !== previous);
  applyArtwork(screen, eligible[Math.floor(Math.random() * eligible.length)]);
}
function updateArtworkStatus() {
  decorationControl.disabled = modeControl.value !== 'manual';
  document.getElementById('shuffle-art').disabled = modeControl.value !== 'random' || paintedTitle();
  if (!currentScreen) return;
  const status = document.getElementById('artwork-status');
  if (paintedTitle()) {
    status.textContent = titleControl.selectedOptions[0].textContent + ' is a complete painted scene. Choose Animated character scene to vary its decoration.';
    return;
  }
  const name = screenElement(currentScreen).dataset.artwork;
  const explanation = modeControl.value === 'random'
    ? 'Chosen on entry; it stays until you leave this screen or choose Show another.'
    : modeControl.value === 'manual' ? 'Your manual choice.' : 'Recommended for this screen.';
  status.textContent = screenNames[currentScreen] + ' · ' + artworkNames[name] + '. ' + explanation;
}
function showScreen(name) {
  if (!Object.hasOwn(captions, name)) name = 'title';
  const entering = name !== currentScreen;
  currentScreen = name;
  document.querySelectorAll('[data-screen]').forEach(button => button.setAttribute('aria-pressed', String(button.dataset.screen === name)));
  document.querySelectorAll('.screen').forEach(section => {
    section.classList.toggle('active', section.dataset.id === name);
    section.setAttribute('aria-hidden', String(section.dataset.id !== name));
  });
  if (modeControl.value === 'recommended') applyArtwork(name, artworkPools[name][0]);
  else if (modeControl.value === 'manual') applyArtwork(name, decorationControl.value);
  else if (entering && !paintedTitle()) chooseRandom(name);
  document.getElementById('scene-caption').textContent = captions[name];
  updateArtworkStatus();
  requestAnimationFrame(() => window.redrawOriginalComponents?.());
}
function setDecoration(name) {
  if (!Object.hasOwn(artworkNames, name)) return;
  decorationControl.value = name;
  Object.keys(artworkPools).forEach(screen => applyArtwork(screen, name));
  updateArtworkStatus();
}
function setTitle(name) {
  const title = screenElement('title');
  const wasAnimated = title.classList.contains('duet');
  title.classList.toggle('picnic', name === 'picnic');
  title.classList.toggle('duet', name === 'duet');
  if (currentScreen === 'title' && name === 'duet' && modeControl.value === 'random' && (!wasAnimated || !artworkPools.title.includes(title.dataset.artwork))) chooseRandom('title');
  updateArtworkStatus();
  requestAnimationFrame(() => window.redrawOriginalComponents?.());
}
modeControl.addEventListener('change', () => {
  if (modeControl.value === 'manual') setDecoration(decorationControl.value);
  else if (modeControl.value === 'recommended') Object.keys(artworkPools).forEach(screen => applyArtwork(screen, artworkPools[screen][0]));
  else if (!paintedTitle()) chooseRandom(currentScreen);
  updateArtworkStatus();
});
document.getElementById('shuffle-art').addEventListener('click', () => {
  if (modeControl.value === 'random' && !paintedTitle()) { chooseRandom(currentScreen); updateArtworkStatus(); }
});
document.querySelectorAll('[data-screen]').forEach(button => button.addEventListener('click', () => showScreen(button.dataset.screen)));
document.getElementById('ratio').addEventListener('change', event => {
  document.getElementById('stage').dataset.ratio = event.target.value;
  requestAnimationFrame(() => window.redrawOriginalComponents?.());
});
titleControl.addEventListener('change', event => setTitle(event.target.value));
decorationControl.addEventListener('change', event => setDecoration(event.target.value));
document.getElementById('candidate-set').addEventListener('change', event => document.getElementById('assets').dataset.set = event.target.value);
document.querySelectorAll('[data-background]').forEach(button => button.addEventListener('click', () => {
  document.getElementById('assets').className = 'asset-grid ' + button.dataset.background;
  document.querySelectorAll('[data-background]').forEach(other => other.setAttribute('aria-pressed', String(other === button)));
}));
if ('scrollRestoration' in history) history.scrollRestoration = 'manual';
const params = new URLSearchParams(location.search);
if (params.get('ratio') === 'classic') {
  document.getElementById('stage').dataset.ratio = 'classic';
  document.getElementById('ratio').value = 'classic';
}
if (Object.hasOwn(artworkNames, params.get('art'))) { decorationControl.value = params.get('art'); modeControl.value = 'manual'; }
if (['recommended','random','manual'].includes(params.get('choice'))) modeControl.value = params.get('choice');
if (['original','picnic','duet'].includes(params.get('title'))) titleControl.value = params.get('title');
setTitle(titleControl.value);
if (modeControl.value === 'manual') setDecoration(decorationControl.value);
if (['cream','checker','dark'].includes(params.get('background'))) document.querySelector('[data-background="' + params.get('background') + '"]').click();
if (params.get('set') === 'all') { document.getElementById('assets').dataset.set = 'all'; document.getElementById('candidate-set').value = 'all'; }
if (params.has('capture')) document.body.classList.add('capture');
showScreen(params.get('capture') || 'title');
if (params.has('capture')) requestAnimationFrame(() => window.scrollTo(0,0));
