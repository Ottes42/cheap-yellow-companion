// Yellow without a browser: runs the WebAssembly module in Node and writes frames as PNG.
// node test/snap.mjs out.png [--lang en|de] [--kind 0|1|2] [--seconds 2] [--gesture wave] [--message "Text"]
//   [--weather rainy] [--decor 0..3] [--occasion halloween] [--date 12-24] [--time 14:30] [--tap x,y] [--hold x,y] [--frames 5]
// --frames N writes N frames spread over the run time (out-1.png …), e.g. for a GIF with ImageMagick.
import { writeFileSync } from 'node:fs';
import { deflateSync } from 'node:zlib';

const args = process.argv.slice(2);
const out = args.shift();
const opt = {};
for (let i = 0; i < args.length; i += 2) opt[args[i].replace(/^--/, '')] = args[i + 1];
if (!out) throw new Error('usage: node test/snap.mjs out.png [...]');

const W = 320, H = 240;
const { default: Yellow } = await import(`../dist/yellow-${opt.lang ?? 'en'}.js`);
const m = await Yellow();
const str = (f) => m.cwrap(f, null, ['string']);
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
async function run(ms) {
  const end = performance.now() + ms;
  while (performance.now() < end) { m._step(); await sleep(16); }
}

const crcTable = Array.from({ length: 256 }, (_, n) => {
  let c = n;
  for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
  return c >>> 0;
});
const crc = (b) => { let c = ~0; for (const x of b) c = crcTable[(c ^ x) & 255] ^ (c >>> 8); return ~c >>> 0; };
const chunk = (type, data) => {
  const len = Buffer.alloc(4); len.writeUInt32BE(data.length);
  const td = Buffer.concat([Buffer.from(type), data]);
  const c = Buffer.alloc(4); c.writeUInt32BE(crc(td));
  return Buffer.concat([len, td, c]);
};

function png(file) {
  const bgra = m.HEAPU8.subarray(m._frame(), m._frame() + W * H * 4);
  const raw = Buffer.alloc((W * 3 + 1) * H);
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const s = (y * W + x) * 4, d = y * (W * 3 + 1) + 1 + x * 3;
      raw[d] = bgra[s + 2]; raw[d + 1] = bgra[s + 1]; raw[d + 2] = bgra[s]; // BGRA → RGB
    }
  }
  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(W, 0); ihdr.writeUInt32BE(H, 4); ihdr[8] = 8; ihdr[9] = 2;
  writeFileSync(file, Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ihdr),
    chunk('IDAT', deflateSync(raw)), chunk('IEND', Buffer.alloc(0))]));
}

m._start(Number(opt.kind ?? 0));
const now = new Date();
const [month, day] = (opt.date ?? `${now.getMonth() + 1}-${now.getDate()}`).split('-').map(Number);
const [hour, minute] = (opt.time ?? `${now.getHours()}:${now.getMinutes()}`).split(':').map(Number);
m._set_time(month, day, hour, minute);
if (opt.decor) m._decor(Number(opt.decor));
if (opt.occasion) str('occasion')(opt.occasion);
if (opt.weather) str('weather')(opt.weather);
await run(Number(opt.warmup ?? 8000)); // let the greeting pass
if (opt.gesture) m.cwrap('gesture', 'number', ['string'])(opt.gesture);
if (opt.message) m.cwrap('message', null, ['string', 'number'])(opt.message.replace(/\\n/g, '\n'), 8);
for (const [kind, ms] of [['tap', 100], ['hold', 1600]]) {
  if (!opt[kind]) continue;
  const [x, y] = opt[kind].split(',').map(Number);
  m._pointer(x, y, 1); await run(ms); m._pointer(x, y, 0);
}
const total = Number(opt.seconds ?? 2) * 1000, frames = Number(opt.frames ?? 1);
for (let i = 1; i <= frames; i++) {
  await run(total / frames);
  png(frames > 1 ? out.replace(/\.png$/, `-${i}.png`) : out);
}
process.exit(0);
