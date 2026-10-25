// Yellow as a Lovelace card: the real firmware (yellow*.h + LVGL) as WebAssembly, 320×240 on a canvas.
// type: custom:yellow-card
// language: en | de                 (on-screen texts; default en)
// character: Okti | Robot | Fox, or the device's character select (select.…_character)
// decor, mood, tension, energy: optional, the device's entities (select or number); the card follows them
// weather: weather.home             (optional: weather condition like on the device, temperature on a long press)
// Selects count by position in their option list (as the firmware does), so their language does not matter.
const W = 320, H = 240;
const KINDS = { Okti: 0, Robot: 1, Roboter: 1, Fox: 2, Fuchs: 2 };
const modules = {}; // one instance per language and page; shortcut: cards with the same language share one companion, load the module per card if that is ever needed

class YellowCard extends HTMLElement {
  setConfig(config) {
    this._config = { character: 'Okti', language: 'en', ...config };
    this._load();
  }

  getCardSize() { return 4; }

  static getStubConfig() { return { character: 'Okti' }; }

  connectedCallback() {
    if (this._canvas) return this._run();
    this.attachShadow({ mode: 'open' }).innerHTML = `
      <style>
        ha-card { overflow: hidden; background: #000; }
        canvas { display: block; width: 100%; aspect-ratio: 4 / 3; touch-action: none; }
      </style>
      <ha-card><canvas width="${W}" height="${H}"></canvas></ha-card>`;
    this._canvas = this.shadowRoot.querySelector('canvas');
    this._ctx = this._canvas.getContext('2d');
    this._image = this._ctx.createImageData(W, H);
    this._setupPointer();
    new IntersectionObserver(([e]) => { this._visible = e.isIntersecting; this._run(); }).observe(this);
    this._load();
  }

  disconnectedCallback() { this._visible = false; }

  // Needs both the config and the canvas; whichever comes second starts it (the order differs by host)
  async _load() {
    if (!this._config || !this._canvas || this._loading) return;
    this._loading = true;
    const lang = this._config.language;
    modules[lang] ??= (async () => {
      const { default: Yellow } = await import(new URL(`yellow-${lang}.js`, import.meta.url));
      const m = await Yellow({ locateFile: (f) => new URL(f, import.meta.url).href });
      m._start(KINDS[this._config.character] ?? 0); // an entity: the character follows with hass
      return m;
    })();
    this._m = await modules[lang];
    this._str = (f) => this._m.cwrap(f, null, ['string']);
    this._last = {};
    this._setTime();
    this._apply();
    this._run();
  }

  _run() {
    if (!this._m || !this._visible || this._running) return;
    this._running = true;
    const frame = () => {
      if (!this._visible || !this.isConnected) { this._running = false; return; }
      if (Date.now() - (this._timeSet ?? 0) > 10000) this._setTime();
      if (this._m._step()) this._draw();
      requestAnimationFrame(frame);
    };
    requestAnimationFrame(frame);
  }

  _draw() {
    const src = this._m.HEAPU8.subarray(this._m._frame(), this._m._frame() + W * H * 4), dst = this._image.data;
    for (let i = 0; i < dst.length; i += 4) { // LVGL's XRGB8888 is BGRA in memory
      dst[i] = src[i + 2]; dst[i + 1] = src[i + 1]; dst[i + 2] = src[i]; dst[i + 3] = 255;
    }
    this._ctx.putImageData(this._image, 0, 0);
  }

  _setTime() {
    const d = new Date();
    this._timeSet = Date.now();
    this._m._set_time(d.getMonth() + 1, d.getDate(), d.getHours(), d.getMinutes());
  }

  _setupPointer() {
    const c = this._canvas;
    const pos = (e) => {
      const r = c.getBoundingClientRect();
      return [Math.round((e.clientX - r.left) * W / r.width), Math.round((e.clientY - r.top) * H / r.height)];
    };
    let down = false;
    c.addEventListener('pointerdown', (e) => { down = true; c.setPointerCapture(e.pointerId); this._m?._pointer(...pos(e), 1); });
    c.addEventListener('pointermove', (e) => { if (down) this._m?._pointer(...pos(e), 1); });
    const up = (e) => { if (!down) return; down = false; this._m?._pointer(...pos(e), 0); };
    c.addEventListener('pointerup', up);
    c.addEventListener('pointercancel', up);
    c.addEventListener('contextmenu', (e) => e.preventDefault()); // long press on phones
  }

  set hass(hass) {
    this._hass = hass;
    this._apply();
  }

  // Pass on changes only: hass arrives on every state change in the whole house
  _apply() {
    if (!this._m || !this._hass) return;
    const c = this._config;
    const ent = (id) => (typeof id === 'string' && id.includes('.') ? this._hass.states[id] : undefined);
    const index = (id) => { const e = ent(id); const i = e?.attributes.options?.indexOf(e.state); return i >= 0 ? i : undefined; };
    const set = (key, value, f) => {
      if (value === undefined || value === 'unknown' || value === 'unavailable' || this._last[key] === value) return;
      this._last[key] = value;
      f(value);
    };
    set('kind', c.character?.includes('.') ? index(c.character) : KINDS[c.character] ?? 0, (v) => this._m._kind(v));
    set('decor', index(c.decor), (v) => this._m._decor(v));
    for (const n of ['mood', 'tension', 'energy'])
      set(n, ent(c[n])?.state, (v) => this._m[`_${n}`](Math.round(Number(v))));
    const w = ent(c.weather);
    if (w) {
      set('weather', w.state, (v) => this._str('weather')(v));
      const t = w.attributes.temperature, unit = w.attributes.temperature_unit ?? '°';
      if (t !== undefined) set('info', `${t}${unit}`, () => this._m.cwrap('info_temp', null, ['number', 'string'])(Number(t), unit));
    }
  }
}

customElements.define('yellow-card', YellowCard);
window.customCards = window.customCards || [];
window.customCards.push({ type: 'yellow-card', name: 'Yellow', description: 'Yellow, the little companion, live in the dashboard' });
