#pragma once
#include <Arduino.h>

// Single-page control panel (http://ESP32_IP/)
const char PANEL_HTML[] PROGMEM = R"UI(<!doctype html>
<html lang=en><head><meta charset=utf-8>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>PC Monitor Control</title>
<style>
:root{color-scheme:dark light;--bg:#0b0f14;--card:#141a22;--line:#2a3441;--tx:#e8eef5;--mut:#9aa7b4;--ac:#4c9aff;--acbg:#0f2a4d;--ok:#46c15a;--warn:#e0a82e;--bad:#ff6b63;--in:#0d1219}
@media (prefers-color-scheme:light){:root{--bg:#f4f6f9;--card:#fff;--line:#d5dce5;--tx:#16202b;--mut:#566474;--ac:#0b5fd1;--acbg:#e3efff;--ok:#1a7f2e;--warn:#9a6700;--bad:#c62828;--in:#fff}}
*{box-sizing:border-box}
[hidden]{display:none!important}
body{margin:0;background:var(--bg);color:var(--tx);font:16px/1.5 system-ui,-apple-system,"Segoe UI",Roboto,sans-serif}
.skip{position:absolute;left:-999px;top:8px;background:var(--ac);color:#fff;padding:8px 14px;border-radius:8px;z-index:50}
.skip:focus{left:8px}
.vh{position:absolute;width:1px;height:1px;overflow:hidden;clip:rect(0 0 0 0);white-space:nowrap}
:focus-visible{outline:3px solid var(--ac);outline-offset:2px;border-radius:6px}
header{position:sticky;top:0;z-index:10;background:var(--bg);border-bottom:1px solid var(--line)}
.bar{max-width:1100px;margin:auto;padding:10px 16px;display:flex;align-items:center;gap:10px;flex-wrap:wrap}
h1{font-size:18px;margin:0;flex:1;min-width:200px}
h1 span{color:var(--mut);font-weight:400}
.chips{display:flex;gap:6px;flex-wrap:wrap}
.chip{padding:3px 10px;border-radius:99px;background:var(--card);border:1px solid var(--line);font-size:13px;color:var(--mut)}
.chip.ok{color:var(--ok);border-color:var(--ok)}.chip.bad{color:var(--bad);border-color:var(--bad)}
.tabs{max-width:1100px;margin:auto;padding:0 10px;display:flex;gap:2px;overflow-x:auto}
.tabs button{background:none;border:0;color:var(--mut);padding:12px 16px;min-height:44px;cursor:pointer;font:inherit;border-bottom:3px solid transparent;white-space:nowrap}
.tabs button[aria-selected=true]{color:var(--tx);border-color:var(--ac);font-weight:600}
main{max-width:1100px;margin:auto;padding:16px}
#apBanner{max-width:1100px;margin:12px auto 0;padding:12px 16px;border-radius:10px;background:var(--acbg);border:1px solid var(--ac);width:calc(100% - 32px)}
.grid{display:grid;gap:14px;grid-template-columns:repeat(auto-fit,minmax(300px,1fr))}
.card{background:var(--card);border:1px solid var(--line);border-radius:14px;padding:16px}
.card.wide{grid-column:1/-1}
.card h2{margin:0 0 12px;font-size:13px;letter-spacing:.06em;text-transform:uppercase;color:var(--mut);font-weight:700}
.mut{color:var(--mut)}
.row{display:flex;align-items:center;gap:10px;flex-wrap:wrap}
.sp{flex:1}
.big{font-size:52px;font-weight:700;line-height:1}
.btn{background:var(--in);color:var(--tx);border:1px solid var(--line);border-radius:10px;padding:8px 16px;min-height:44px;cursor:pointer;font:inherit}
.btn:hover{border-color:var(--ac)}
.btn.pri{background:var(--ac);border-color:var(--ac);color:#fff}
.btn.danger{color:var(--bad);border-color:var(--bad);background:transparent}
.btn:disabled{opacity:.5;cursor:not-allowed}
input[type=number],input[type=text],input[type=password],select{background:var(--in);color:var(--tx);border:1px solid var(--line);border-radius:10px;padding:8px 12px;min-height:44px;font:inherit;width:100%}
input[type=number]{width:96px}
input[type=range]{width:100%;min-height:32px;accent-color:var(--ac)}
label.f{display:block;margin:12px 0 4px;color:var(--mut);font-size:14px}
.hint{color:var(--mut);font-size:14px;margin:8px 0 0}
.pgrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(120px,1fr));gap:8px}
.pbtn{background:var(--in);border:2px solid var(--line);color:var(--tx);border-radius:12px;padding:12px 8px;min-height:64px;cursor:pointer;font:inherit;text-align:center}
.pbtn small{display:block;color:var(--mut)}
.pbtn[aria-pressed=true]{border-color:var(--ac);background:var(--acbg)}
.pbtn.off{opacity:.6}
.sw{position:relative;display:inline-flex;align-items:center;gap:10px;cursor:pointer;min-height:44px}
.sw input{position:absolute;opacity:0;width:44px;height:28px;margin:0;cursor:pointer}
.sw .trk{position:relative;width:46px;height:26px;border-radius:99px;background:var(--line);transition:background .15s;flex:none}
.sw .trk:before{content:"";position:absolute;width:20px;height:20px;left:3px;top:3px;border-radius:50%;background:#fff;transition:transform .15s}
.sw input:checked+.trk{background:var(--ac)}
.sw input:checked+.trk:before{transform:translateX(20px)}
.sw input:focus-visible+.trk{outline:3px solid var(--ac);outline-offset:2px}
.oo{font-size:12px;padding:2px 8px;border-radius:6px;min-width:40px;text-align:center;border:1px solid var(--line)}
.oo.on{color:var(--ok);border-color:var(--ok)}.oo.off{color:var(--mut)}
.prow{display:flex;align-items:center;gap:14px;padding:8px 0;border-bottom:1px solid var(--line);flex-wrap:wrap}
.prow .pn{flex:1;font-weight:600;min-width:110px}
.meter{height:10px;background:var(--line);border-radius:6px;overflow:hidden;margin:6px 0 12px}
.meter i{display:block;height:100%;width:0;background:var(--ok);transition:width .4s}
table{width:100%;border-collapse:collapse}
td,th{padding:8px 6px;border-bottom:1px solid var(--line);text-align:left}
th{color:var(--mut);font-weight:500;font-size:13px}
canvas{width:100%;height:80px;display:block;margin-top:10px}
#dz,#dzI{border:2px dashed var(--line);border-radius:14px;padding:30px 16px;text-align:center;color:var(--mut);cursor:pointer;background:transparent;width:100%;font:inherit;min-height:120px}
#dz:hover,#dz.hot,#dzI:hover,#dzI.hot{border-color:var(--ac);background:var(--acbg);color:var(--tx)}
#upl,#uplI{list-style:none;margin:10px 0 0;padding:0}
.urow{display:flex;align-items:center;gap:10px;padding:6px 0;font-size:14px;flex-wrap:wrap}
.urow .un{flex:1;min-width:120px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.pb{width:120px;height:8px;background:var(--line);border-radius:4px;overflow:hidden}
.pb i{display:block;height:100%;width:0;background:var(--ac)}
.ust{min-width:110px;font-size:13px}
#nets{list-style:none;margin:8px 0 0;padding:0;max-height:240px;overflow:auto}
#nets button{width:100%;display:flex;justify-content:space-between;gap:8px;background:none;border:0;border-bottom:1px solid var(--line);color:var(--tx);padding:10px 6px;min-height:44px;cursor:pointer;font:inherit;text-align:left}
#nets button:hover{background:var(--acbg)}
#toast{position:fixed;left:50%;bottom:24px;transform:translate(-50%,20px);background:#1d6b2c;color:#fff;padding:12px 20px;border-radius:12px;opacity:0;transition:.25s;pointer-events:none;z-index:40;max-width:90vw}
#toast.show{opacity:1;transform:translate(-50%,0)}
#toast.bad{background:#a32b27}
@media (prefers-reduced-motion:reduce){*{transition:none!important}}

/* ---- web themes (header selector) ---- */
body{font-size:calc(16px*var(--fs,1))}
html[data-theme=dark]{--bg:#0b0f14;--card:#141a22;--line:#2a3441;--tx:#e8eef5;--mut:#9aa7b4;--ac:#4c9aff;--acbg:#0f2a4d;--in:#0d1219;color-scheme:dark}
html[data-theme=light]{--bg:#f4f6f9;--card:#fff;--line:#d5dce5;--tx:#16202b;--mut:#566474;--ac:#0b5fd1;--acbg:#e3efff;--ok:#1a7f2e;--warn:#9a6700;--bad:#c62828;--in:#fff;color-scheme:light}
html[data-theme=midnight]{--bg:#000;--card:#0a0a0c;--line:#26262c;--tx:#f2f2f5;--mut:#9a9aa6;--ac:#7aa7ff;--acbg:#0b1530;--in:#050507;color-scheme:dark}
html[data-theme=ocean]{--bg:#06141f;--card:#0b2233;--line:#1b4560;--tx:#e6f6ff;--mut:#8fb8cf;--ac:#2fd0ff;--acbg:#08384d;--in:#071a28;color-scheme:dark}
html[data-theme=forest]{--bg:#0a140e;--card:#10201a;--line:#24402f;--tx:#e8f5ea;--mut:#95b5a0;--ac:#4fd88a;--acbg:#0f3a24;--in:#0b1913;color-scheme:dark}
html[data-theme=sunset]{--bg:#170d1f;--card:#22132e;--line:#46285c;--tx:#fff0f4;--mut:#c9a4bd;--ac:#ff7a90;--acbg:#4a1d3f;--in:#1a0f24;color-scheme:dark}
html[data-theme=contrast]{--bg:#000;--card:#000;--line:#fff;--tx:#fff;--mut:#e0e0e0;--ac:#ffe600;--acbg:#1a1a00;--ok:#00ff5a;--warn:#ffc400;--bad:#ff5a5a;--in:#000;color-scheme:dark}
html[data-theme=contrast] .card,html[data-theme=contrast] .btn,html[data-theme=contrast] select,html[data-theme=contrast] input{border-width:2px}
.ap{display:flex;gap:6px;align-items:center}
.ap select{min-height:36px;padding:4px 8px;border-radius:8px;background:var(--in);color:var(--tx);border:1px solid var(--line);font:inherit;font-size:13px}
.ap .btn{min-height:36px;padding:2px 10px;font-weight:700}
.kbd{display:inline-block;border:1px solid var(--line);border-bottom-width:2px;border-radius:5px;padding:0 6px;font:12px/1.6 ui-monospace,monospace;background:var(--in)}
.big2{font-size:22px;font-weight:700}
.pl{display:flex;gap:10px;flex-wrap:wrap}.pl .btn{flex:1;min-width:120px;font-size:17px}
input[type=range]{width:100%;accent-color:var(--ac);min-height:32px}
.rm-wrap{display:grid;gap:16px;grid-template-columns:minmax(300px,520px) minmax(280px,1fr);align-items:start}
@media (max-width:820px){.rm-wrap{grid-template-columns:1fr}}
.rm-tabs{display:flex;gap:6px;flex-wrap:wrap;margin-bottom:10px}
.rm-tab{background:var(--in);color:var(--tx);border:2px solid var(--line);border-radius:99px;padding:6px 16px;min-height:40px;font:inherit;font-weight:600;cursor:pointer}
.rm-tab[aria-pressed=true]{border-color:var(--ac);background:var(--acbg)}
.rm-tab.add{border-style:dashed;color:var(--mut)}
.rm-screen{background:#000;border:3px solid var(--line);border-radius:16px;padding:8px;display:grid;gap:6px;align-content:start}
.rm-hd{display:flex;justify-content:space-between;align-items:center;background:#000a5c;color:#fff;border-radius:6px;padding:2px 10px;font-weight:700;font-size:13px}
.rm-hd i{width:10px;height:10px;border-radius:50%;background:#2ecc71;display:inline-block}
.rm-strip{display:grid;grid-template-columns:56px 1fr 56px;gap:6px;align-items:center}
.rm-strip button{background:#2a2f45;color:#ffd84a;border:2px solid #ffd84a;border-radius:6px;font:700 18px/1 system-ui;min-height:34px;cursor:pointer}
.rm-strip b{text-align:center;color:#3ee6ff;font-size:18px}.rm-strip b small{color:#777;font-size:12px;margin-left:10px}
.rm-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:6px}
.rm-cell{min-height:56px;border-radius:6px;border:2px solid #3ee6ff;background:#000a5c;color:#fff;font:700 13px/1.15 system-ui;cursor:pointer;padding:4px;word-break:break-word;display:flex;align-items:center;justify-content:center;text-align:center}
.rm-cell.danger{border-color:#ff4d4d;color:#ffa44d}
.rm-cell.nav{background:#2a2f45;border-color:#ffd84a;color:#ffd84a;font-size:11px;flex-direction:column;gap:2px}
.rm-cell.nav b{color:#fff;font-size:12px}
.rm-cell.empty{background:#000;border:2px dashed #29323d;color:#5c6b7a;font-size:22px}
.rm-cell[aria-pressed=true]{outline:3px solid #fff;outline-offset:1px;box-shadow:0 0 0 5px var(--ac)}
.rm-cell:disabled{opacity:.5;cursor:default}
.rm-toast{background:#10101a;border:2px solid #2ecc71;border-radius:6px;color:#bbb;font-size:12px;padding:3px 8px;min-height:24px}
.rm-ins h3{margin:0 0 10px;font-size:18px}
.rm-ins .row+.row{margin-top:10px}
.rm-bar{position:sticky;bottom:0;z-index:5;display:flex;gap:10px;align-items:center;flex-wrap:wrap;background:var(--card);border:1px solid var(--line);border-radius:14px;padding:10px 14px;margin-top:14px}
.rm-dirty{color:var(--warn);font-weight:600}
.rm-help{display:grid;grid-template-columns:auto 1fr;gap:4px 12px;font-size:14px;color:var(--mut);margin:12px 0 0}
.rm-help code,.rm-ins code{background:var(--in);border:1px solid var(--line);border-radius:5px;padding:0 5px}
@media (prefers-reduced-motion:reduce){*{transition:none!important;animation:none!important;scroll-behavior:auto!important}}
</style>
<script>try{var t=localStorage.wt;if(t&&t!=="auto")document.documentElement.dataset.theme=t;var f=localStorage.wfs;if(f)document.documentElement.style.setProperty("--fs",f)}catch(e){}</script></head><body>
<a class=skip href="#main">Skip to content</a>
<header role=banner>
<div class=bar><h1>PC Monitor <span>Control Panel</span></h1>
<div class=ap role=group aria-label="Appearance"><label class=vh for=wtheme>Web theme</label><select id=wtheme><option value=auto>Theme: Auto</option><option value=dark>Dark</option><option value=light>Light</option><option value=midnight>Midnight (AMOLED)</option><option value=ocean>Ocean</option><option value=forest>Forest</option><option value=sunset>Sunset</option><option value=contrast>High contrast</option></select><button class=btn id=fsm type=button aria-label="Smaller text">A&minus;</button><button class=btn id=fsp type=button aria-label="Larger text">A+</button></div>
<div class=chips role=status aria-label="Device status"><span class=chip id=cPc>PC --</span><span class=chip id=cWifi>WiFi --</span><span class=chip id=cIp>--</span><span class=chip id=cUp>--</span></div></div>
<div class=tabs role=tablist aria-label="Sections" id=nav>
<button role=tab id=tab-dash aria-controls=t-dash aria-selected=true>Dashboard</button>
<button role=tab id=tab-disp aria-controls=t-disp aria-selected=false tabindex=-1>Display &amp; Timing</button>
<button role=tab id=tab-rmt aria-controls=t-rmt aria-selected=false tabindex=-1>PC Remote</button>
<button role=tab id=tab-image aria-controls=t-image aria-selected=false tabindex=-1>Image</button>
<button role=tab id=tab-video aria-controls=t-video aria-selected=false tabindex=-1>Video</button>
<button role=tab id=tab-sys aria-controls=t-sys aria-selected=false tabindex=-1>System</button>
</div></header>
<div id=apBanner role=alert hidden><b>Setup mode.</b> Open the <b>System</b> tab, scan, pick your WiFi and press Connect.</div>
<main id=main>

<section role=tabpanel id=t-dash aria-labelledby=tab-dash tabindex=0>
<div class=grid>
<div class=card><h2>Screen control</h2>
<div class=pgrid id=pgrid role=group aria-label="Show page on display"></div>
<div class=row style="margin-top:14px"><button class=btn id=bPrev>&larr; Previous</button><button class=btn id=bNext>Next &rarr;</button><span class=sp></span>
<label class=sw><input type=checkbox role=switch id=autoSw><span class=trk aria-hidden=true></span><span class=lbl>Auto-switch</span><b class="oo off" aria-hidden=true>OFF</b></label></div>
<p class=hint>Auto-switch ON thakle page gulo Display &amp; Timing-er shomoy onujayi ghure.</p></div>
<div class=card><h2>CPU</h2><div class=big id=cpu>--</div><div class=mut id=cpuSub>--</div><canvas id=cv height=80 role=img aria-label="CPU usage history"></canvas></div>
<div class=card><h2>Temperature</h2>
<div class=row><span class=mut>CPU</span><span class=sp></span><b id=tc>--</b></div><div class=meter><i id=tcb></i></div>
<div class=row><span class=mut>GPU</span><span class=sp></span><b id=tg>--</b></div><div class=meter><i id=tgb></i></div>
<div class=mut id=gl>GPU: --</div></div>
<div class=card><h2>Memory &amp; Disk</h2>
<div class=row><span class=mut>RAM</span><span class=sp></span><b id=ramT>--</b></div><div class=meter><i id=ramB></i></div>
<div class=row><span class=mut>Disk</span><span class=sp></span><b id=dskT>--</b></div><div class=meter><i id=dskB></i></div></div>
<div class=card><h2>Network</h2>
<div class=row><span class=mut>Upload</span><span class=sp></span><b id=nUp>--</b></div>
<div class=row><span class=mut>Download</span><span class=sp></span><b id=nDn>--</b></div>
<div class=row><span class=mut>Ping</span><span class=sp></span><b id=nPing>--</b></div>
<div class=row><span class=mut>Total RX / TX</span><span class=sp></span><b id=nTot>--</b></div>
<div class=row><span class=mut>PC uptime</span><span class=sp></span><b id=pcUp>--</b></div></div>
<div class=card><h2>Disk I/O</h2>
<div class=row><span class=mut>Read</span><span class=sp></span><b id=dRt>--</b></div><div class=meter><i id=dRb></i></div>
<div class=row><span class=mut>Write</span><span class=sp></span><b id=dWt>--</b></div><div class=meter><i id=dWb></i></div>
<div class=row><span class=mut>Swap</span><span class=sp></span><b id=swp>--</b></div>
<div class=row><span class=mut>Battery</span><span class=sp></span><b id=bat>--</b></div></div>
<div class=card><h2>Active window</h2><div id=win style="font-size:18px;font-weight:600;word-break:break-word">--</div><div class=mut id=wapp></div><div class=mut id=clock style="margin-top:8px">--</div></div>
<div class=card><h2>Drives</h2><div id=drv>--</div></div>
<div class=card><h2>Top RAM</h2><table><tbody id=rps></tbody></table></div>
<div class=card><h2>Top I/O (read+write)</h2><table><tbody id=dps></tbody></table></div>
<div class="card wide"><h2>Top processes <span id=nproc></span></h2>
<table><thead><tr><th scope=col>Process</th><th scope=col>CPU</th><th scope=col>RAM</th></tr></thead><tbody id=procs></tbody></table></div>
</div></section>

<section role=tabpanel id=t-disp aria-labelledby=tab-disp tabindex=0 hidden>
<div class=grid>
<div class="card wide"><h2>Player</h2>
<div class=row><div class=sp><div class=big2 id=plName>Nothing playing</div><div class=mut id=plInfo aria-live=polite></div></div></div>
<div class=pl style="margin-top:12px"><button class=btn id=bPrevM type=button>&#9198; Previous media</button><button class=btn id=bNextM type=button>Next media &#9197;</button><button class=btn id=bPrevP type=button>&larr; Previous page</button><button class=btn id=bNextP type=button>Next page &rarr;</button></div>
<p class=hint>Shortcuts: <span class=kbd>Alt</span>+<span class=kbd>,</span> / <span class=kbd>.</span> prev / next media &middot; <span class=kbd>Alt</span>+<span class=kbd>[</span> / <span class=kbd>]</span> prev / next page &middot; <span class=kbd>Alt</span>+<span class=kbd>&uarr;</span> / <span class=kbd>&darr;</span> volume. On the touch screen: long-press the left / right edge = previous / next media; drag up/down on the right half = volume, left half = brightness.</p></div>
<div class=card><h2>Device theme &amp; sound</h2>
<label class=f for=devTheme>Screen colour theme</label>
<select id=devTheme></select>
<p class=hint>Rong-er theme shob page-e lage (video / photo-te na).</p>
<label class=f for=vol>Volume <output id=volv for=vol>70%</output></label>
<input type=range id=vol min=0 max=100 step=2 value=70 aria-describedby=volHint>
<p class=hint id=volHint>Sound sudhu AVI video-te (web theke upload korle auto sound thake). Speaker = CYD-r speaker connector (GPIO26).</p></div>
<div class=card><h2>Pages &amp; timing</h2><div id=prow></div>
<p class=hint>Switch OFF korle oi page auto-switch-e skip hobe. Number = oi page koto second thakbe. Sob change sathe sathe save hoy.</p></div>
<div class=card><h2>Alerts</h2>
<label class=sw><input type=checkbox role=switch id=alOn><span class=trk aria-hidden=true></span><span class=lbl>Alert when PC gets too hot / busy</span><b class="oo off" aria-hidden=true>OFF</b></label>
<label class=sw><input type=checkbox role=switch id=alJump><span class=trk aria-hidden=true></span><span class=lbl>Jump to the page + stop auto-switch</span><b class="oo off" aria-hidden=true>OFF</b></label>
<label class=f for=alCpuT>CPU temp limit (&deg;C)</label><input type=number id=alCpuT min=40 max=110>
<label class=f for=alGpuT>GPU temp limit (&deg;C)</label><input type=number id=alGpuT min=40 max=110>
<label class=f for=alCpu>CPU usage limit (%, 5 sec average)</label><input type=number id=alCpu min=30 max=100>
<label class=f for=alRam>RAM usage limit (%)</label><input type=number id=alRam min=30 max=100>
<p class=hint id=alNow>Limit par hole display-er header lal hoye blink korbe.</p></div>
<div class=card><h2>Night mode</h2>
<label class=sw><input type=checkbox role=switch id=ntOn><span class=trk aria-hidden=true></span><span class=lbl>Dim the display at night</span><b class="oo off" aria-hidden=true>OFF</b></label>
<label class=f for=ntFrom>From hour (0-23, PC clock)</label><input type=number id=ntFrom min=0 max=23>
<label class=f for=ntTo>To hour (0-23)</label><input type=number id=ntTo min=0 max=23>
<label class=f for=ntBr>Night brightness <b id=ntBrv></b></label><input type=range id=ntBr min=5 max=255>
<p class=hint>Night mode-e normal Brightness slider-er chaite kom ta apply hoy. Alert aslei screen abar full brightness hoy.</p></div>
<div class=card><h2>Screen</h2>
<label class=f for=bright>Brightness <b id=brv></b></label><input type=range id=bright min=5 max=255>
<label class=f for=rot>Orientation</label><select id=rot><option value=1>Landscape</option><option value=3>Landscape (flipped 180&deg;)</option></select>
<label class=sw><input type=checkbox role=switch id=clk12><span class=trk aria-hidden=true></span><span class=lbl>12-hour clock (AM/PM)</span><b class="oo off" aria-hidden=true>OFF</b></label>
<label class=sw><input type=checkbox role=switch id=clkSec><span class=trk aria-hidden=true></span><span class=lbl>Show seconds on the clock</span><b class="oo off" aria-hidden=true>OFF</b></label>
<label class=f for=gifSkip>Native GIF playback</label>
<select id=gifSkip><option value=1>Smooth: frame skip kore speed thik rakhe</option><option value=0>All frames: slow hole dhire cholbe</option></select>
<div class=row style="margin-top:18px"><span class=sp></span><button class="btn danger" id=bReset>Reset display settings</button></div></div>
</div></section>

<section role=tabpanel id=t-rmt aria-labelledby=tab-rmt tabindex=0 hidden>
<div class=grid>
<div class="card wide"><h2>PC remote &mdash; CYD buttons</h2>
<p class=hint style="margin:0 0 12px">Niche CYD-r screen-er preview. <b>Tab</b> chip chapo tab bodlate, <b>button</b> chapo edit korte, <b>+</b> chapo notun button add korte. Protita tab-e 10-ta button + <span class=kbd>PREV</span> <span class=kbd>NEXT</span> tab button thake. Kaj shesh hole <b>Save changes</b>.</p>
<div class=rm-wrap>
<div>
<div class=rm-tabs id=rmTabs role=group aria-label="Remote tabs"></div>
<div class=rm-screen aria-label="CYD screen preview">
<div class=rm-hd><span>PC REMOTE</span><i title="PC agent link"></i></div>
<div class=rm-strip><button type=button id=rmPrevT aria-label="Previous tab">&lt;</button><b id=rmTabName></b><button type=button id=rmNextT aria-label="Next tab">&gt;</button></div>
<div class=rm-grid id=rmGrid role=group aria-label="Button grid"></div>
<div class=rm-toast id=rmToast aria-hidden=true>popup bar: ekhane CYD-te SENT / ERROR dekhabe</div>
</div></div>
<div class="rm-ins" id=rmIns aria-live=polite>
<div id=rmTabEd class=card style="background:var(--in);margin-bottom:12px"><h3>Tab</h3>
<div class=row><div class=sp><label class=f for=rmName style="margin-top:0">Tab name</label><input id=rmName type=text maxlength=10 autocomplete=off></div></div>
<div class=row style="margin-top:10px"><button class=btn type=button id=rmTabL>&larr; Move left</button><button class=btn type=button id=rmTabR>Move right &rarr;</button><button class="btn danger" type=button id=rmTabDel>Delete tab</button></div></div>
<div id=rmBtnEd class=card style="background:var(--in)"><h3 id=rmBtnTitle>Button</h3>
<div id=rmBtnForm>
<label class=f for=rmAct style="margin-top:0">Action</label><select id=rmAct></select>
<div id=rmCustomW hidden><label class=f for=rmCustom id=rmCustomL>Value</label><input id=rmCustom type=text maxlength=44 autocomplete=off spellcheck=false></div>
<label class=f for=rmLabel>Label on the button (max 12)</label><input id=rmLabel type=text maxlength=12 autocomplete=off>
<div class=row style="margin-top:12px"><label class=sw><input type=checkbox role=switch id=rmConfirm><span class=trk aria-hidden=true></span><span class=lbl>Confirm (duibar tap)</span></label></div>
<div class=row style="margin-top:12px"><button class="btn pri" type=button id=rmTest>Test on PC</button><button class=btn type=button id=rmL aria-label="Move button earlier">&larr;</button><button class=btn type=button id=rmR aria-label="Move button later">&rarr;</button><button class="btn danger" type=button id=rmDel>Remove</button></div>
</div>
<p class=hint id=rmNone>Ekta button chapo ba <b>+</b> chapo.</p>
</div>
<div class=rm-help><b>keys:</b><span><code>keys:ctrl+shift+n</code> kono shortcut</span><b>url:</b><span><code>url:https://youtube.com</code> link khola</span><b>app:</b><span><code>app:notepad</code> <code>app:chrome</code> <code>app:C:\path\x.exe</code> jekono app</span></div>
</div></div>
<div class=rm-bar><button class="btn pri" type=button id=rmSave>Save changes</button><button class=btn type=button id=rmRevert>Undo changes</button><button class=btn type=button id=rmReset>Default layout</button><span id=rmState class=mut role=status></span></div>
</div>
<div class=card><h2>Agent token</h2>
<p class=hint style="margin:0">PC-te <code>run_pc_agent.bat</code> chalale console-e Token dekhabe (file: <code>pc_agent_token.txt</code>). Port = PC connection-er Port-i (ek-i).</p>
<div class=row><div class=sp><label class=f for=rmtTok>Token <span class=mut id=rmtTokSt></span></label><input id=rmtTok type=text autocomplete=off spellcheck=false placeholder="e.g. 65b6fc3b"></div><button class="btn pri" id=rmtSave type=button>Save token</button></div></div>
</div></section>

<section role=tabpanel id=t-image aria-labelledby=tab-image tabindex=0 hidden>
<div class=grid>
<div class="card wide"><h2>Now showing</h2>
<div class=row><div id=nowTxt class=sp>Nothing loaded.</div><button class=btn id=bShow>Show on display</button><button class="btn danger" id=bClear>Clear</button></div>
<p class=hint>Media flash-e save hoy na. Upload korle ESP32-r RAM-e giye sathe sathe display-te dekhay. Reboot korle muche jay.</p></div>
<div class=card><h2>Image options</h2>
<label class=f for=fit>Image fit</label>
<select id=fit><option value=contain>Fit inside (no crop)</option><option value=cover selected>Fill screen 320x240 (crop)</option><option value=stretch>Stretch to 320x240</option></select>
<label class=f for=destI>Upload destination</label>
<select id=destI><option value=ram>Show now (RAM, lost on reboot)</option><option value=sdplay>Save to SD card + show</option><option value=sdonly>Save to SD card only</option></select>
<label class=sw><input type=checkbox role=switch id=holdSw><span class=trk aria-hidden=true></span><span class=lbl>Hold on uploaded media</span><b class="oo off" aria-hidden=true>OFF</b></label></div>
<div class=card><h2>Upload image</h2>
<button type=button id=dzI>Drop an image here, click to choose, or paste (Ctrl+V)</button>
<input type=file id=fiI accept="image/*" multiple hidden>
<ul id=uplI aria-label="Image upload progress"></ul>
<p class=hint>JPG, PNG, WebP, BMP... browser-ei 320x240 e auto convert hoy (CYD). Animated GIF/WebP dile Video tab use koro.</p></div>
</div></section>

<section role=tabpanel id=t-video aria-labelledby=tab-video tabindex=0 hidden>
<div class=grid>
<div class="card wide"><h2>Now showing</h2>
<div class=row><div id=nowTxtV class=sp>Nothing loaded.</div><button class=btn id=bShowV>Show on display</button><button class="btn danger" id=bClearV>Clear</button></div></div>
<div class=card><h2>Video options</h2>
<label class=f for=vq>Video / clip quality</label>
<select id=vq><option value=best selected>BEST quality - 320x240, 10 fps</option><option value=full>High - 320x240, 12 fps</option><option value=smooth>Smooth - 320x214, 12 fps</option><option value=balanced>Balanced - 280x210, 15 fps</option><option value=fast>Fast - 240x160, 20 fps (smoothest)</option><option value=wide>Wide - 320x180, 15 fps</option><option value=full18>Small - 160x120, 25 fps</option></select>
<p class=hint id=vqHint></p>
<label class=f for=audioOn><input type=checkbox id=audioOn checked> Video-r sathe sound rakho (AVI + 22 kHz mono)</label>
<label class=f for=animMode>GIF / animation</label>
<select id=animMode><option value=clip>Convert to smooth clip (recommended)</option><option value=native>Send original GIF</option></select>
<label class=f for=dest>Upload destination</label>
<select id=dest><option value=ram>Play now (RAM, lost on reboot)</option><option value=sdplay>Save to SD card + play</option><option value=sdonly>Save to SD card only</option></select>
<label class=f for=vmax>Max clip length (seconds)</label><input type=number id=vmax value=15 min=1 max=3600></div>
<div class=card><h2>Upload video / GIF</h2>
<button type=button id=dz>Drop a video or GIF here, click to choose, or paste (Ctrl+V)</button>
<input type=file id=fi accept="video/*,.avi,.mkv,.mp4,.mov,.webm,image/gif,image/webp,image/apng" multiple hidden>
<ul id=upl aria-label="Video upload progress"></ul>
<p class=hint>Video/GIF browser-eii smooth clip-e convert hoy. Ekbar-e ekta media dekhay, notun upload ager-ta replace kore.</p></div>
<div class="card wide" id=sdCard><h2>SD card</h2>
<div class=row><span class=chip id=sdChip>SD --</span><span id=sdInfo class=sp></span><button class=btn id=bSdRef>Refresh / mount</button></div>
<div class=meter aria-hidden=true><i id=sdBar></i></div>
<ul id=sdList aria-label="Videos on SD card"></ul>
<div class=row style="margin-top:12px"><button class=btn id=bPlayAll>Play all (playlist)</button><button class=btn id=bStopAll>Stop playlist</button></div>
<label class=f for=sdLoops>Loops per video (playlist)</label><input type=number id=sdLoops min=1 max=50 style="width:110px">
<label class=f for=sdStill>Seconds per still image (playlist)</label><input type=number id=sdStill min=2 max=3600 style="width:110px">
<label class=sw><input type=checkbox role=switch id=sdPre><span class=trk aria-hidden=true></span><span class=lbl>Preload small clips into RAM</span><b class="oo off" aria-hidden=true>OFF</b></label>
<p class=hint>SD card-e jawa video ESP32 theke read-ahead buffer diye cholbe. Files: /media folder (.mjv .rgb .gif .jpg).</p></div>
</div></section>

<section role=tabpanel id=t-sys aria-labelledby=tab-sys tabindex=0 hidden>
<div class=grid>
<div class=card><h2>PC connection</h2>
<label class=f for=pcIp>PC IP (pc_agent.py cholche ja)</label><input type=text id=pcIp inputmode=decimal autocomplete=off>
<label class=f for=pcPort>Port</label><input type=number id=pcPort min=1 max=65535 style="width:130px">
<label class=f for=pcPoll>Refresh interval (ms)</label><input type=number id=pcPoll min=300 max=10000 style="width:130px">
<div class=row style="margin-top:16px"><span class=sp></span><button class="btn pri" id=bSavePc>Save</button></div></div>
<div class=card><h2>WiFi</h2>
<div class=row><button class=btn id=bScan>Scan networks</button><span id=scanSt class=mut role=status></span></div>
<ul id=nets aria-label="Available WiFi networks"></ul>
<label class=f for=ssid>Network name (SSID)</label><input type=text id=ssid autocomplete=off>
<label class=f for=pass>Password</label><input type=password id=pass autocomplete=off>
<label class=sw style="margin-top:6px"><input type=checkbox id=showPw><span class=lbl>Show password</span></label>
<div class=row style="margin-top:14px"><button class="btn" id=bApStop hidden>Finish setup (close hotspot)</button><span class=sp></span><button class="btn pri" id=bConn>Connect</button></div>
<p class=hint id=wifiMsg role=status>Connect na hole device nijei "PC-Monitor-Setup" hotspot chalu kore. Boot button 4 sec dhorleo setup mode ashe.</p></div>
<div class=card><h2>Device</h2><table><tbody id=sysInfo></tbody></table>
<div class=row style="margin-top:14px"><span class=sp></span><button class="btn danger" id=bReboot>Reboot</button></div></div>
</div></section>
</main>
<div id=toast role=status aria-live=polite></div>

<script>
const $=s=>document.querySelector(s),$$=s=>[...document.querySelectorAll(s)];
const sleep=ms=>new Promise(r=>setTimeout(r,ms));
const esc=s=>String(s).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
let S=null,formInit=false,busy=false;
function toast(m,bad){const t=$('#toast');t.textContent=m;t.className='show'+(bad?' bad':'');clearTimeout(toast.h);toast.h=setTimeout(()=>{t.className=''},2800)}
async function api(p,o,quiet){try{const r=await fetch(p,o);const t=await r.text();try{return JSON.parse(t)}catch(e){return{ok:r.ok}}}catch(e){if(!quiet)toast('Device unreachable',1);return null}}
const post=(p,b)=>api(p,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(b||{})});
const col=p=>p<60?'#46c15a':p<85?'#e0a82e':'#ff6b63';
const tcol=t=>t<60?'#46c15a':t<75?'#e0a82e':t<85?'#f0883e':'#ff6b63';
const fmtB=b=>b>=1048576?(b/1048576).toFixed(1)+' MB':Math.round(b/1024)+' KB';
const rate=k=>k>=1024?(k/1024).toFixed(1)+' MB/s':Math.round(k)+' KB/s';
const dur=s=>{const d=Math.floor(s/86400),h=Math.floor(s%86400/3600),m=Math.floor(s%3600/60);return(d?d+'d ':'')+h+'h '+m+'m'};
function setBar(id,p,c){const e=$(id);e.style.width=Math.min(100,Math.max(0,p))+'%';e.style.background=c}

/* ---------- tabs (keyboard accessible) ---------- */
const TABS=['dash','disp','rmt','image','video','sys'];
function tab(t,focus){TABS.forEach(k=>{const on=k===t,b=$('#tab-'+k);b.setAttribute('aria-selected',on);b.tabIndex=on?0:-1;$('#t-'+k).hidden=!on});history.replaceState(null,'','#'+t);if(focus)$('#tab-'+t).focus()}
TABS.forEach(k=>{$('#tab-'+k).onclick=()=>tab(k)});
$('#nav').addEventListener('keydown',e=>{const i=TABS.indexOf(document.activeElement.id.replace('tab-',''));if(i<0)return;let n=-1;
 if(e.key==='ArrowRight')n=(i+1)%TABS.length;else if(e.key==='ArrowLeft')n=(i+TABS.length-1)%TABS.length;else if(e.key==='Home')n=0;else if(e.key==='End')n=TABS.length-1;
 if(n>=0){e.preventDefault();tab(TABS[n],true)}});

/* ---------- switches with ON/OFF text + message ---------- */
function syncSw(inp){const b=inp.closest('label').querySelector('.oo');if(b){b.textContent=inp.checked?'ON':'OFF';b.className='oo '+(inp.checked?'on':'off')}}
function onSw(inp,name,fn){inp.addEventListener('change',async()=>{syncSw(inp);const r=await fn(inp.checked);toast(name+': '+(inp.checked?'ON':'OFF'),!(r&&r.ok))})}
const swHtml=(id,label)=>'<label class=sw><input type=checkbox role=switch id='+id+'><span class=trk aria-hidden=true></span><span class=vh>'+label+'</span><b class="oo off" aria-hidden=true>OFF</b></label>';

/* ---------- dashboard controls ---------- */
$('#bPrev').onclick=()=>{api('/api/prev').then(()=>loadState())};
$('#bNext').onclick=()=>{api('/api/next').then(()=>loadState())};
onSw($('#autoSw'),'Auto-switch',v=>post('/api/settings',{auto:v,save:true}).then(x=>{loadState();return x}));
onSw($('#holdSw'),'Hold on uploaded media',v=>post('/api/settings',{hold:v,save:true}));

function buildPages(s){
 const g=$('#pgrid');g.innerHTML='';
 s.pages.forEach((p,i)=>{const b=document.createElement('button');b.className='pbtn';b.id='pb'+i;b.setAttribute('aria-pressed','false');b.innerHTML='<b>'+esc(p.n)+'</b><small></small>';b.onclick=()=>{api('/api/page?n='+i).then(()=>loadState())};g.appendChild(b)});
 const box=$('#prow');box.innerHTML='';
 s.pages.forEach((p,i)=>{const r=document.createElement('div');r.className='prow';
  r.innerHTML='<span class=pn>'+esc(p.n)+'</span>'+swHtml('pon'+i,'Show '+esc(p.n)+' in rotation')+'<span><label class=vh for=psec'+i+'>'+esc(p.n)+' seconds</label><input type=number id=psec'+i+' min=2 max=3600 inputmode=numeric> <span class=mut>sec</span></span>';
  box.appendChild(r);
  onSw($('#pon'+i),p.n+' page',v=>post('/api/settings',{page:{i:i,on:v},save:true}).then(x=>{loadState();return x}));
  $('#psec'+i).addEventListener('change',async e=>{let v=Math.round(+e.target.value);if(!(v>=2))v=2;if(v>3600)v=3600;e.target.value=v;
   const x=await post('/api/settings',{page:{i:i,s:v},save:true});toast(p.n+': '+v+' sec'+(x&&x.ok?' (saved)':' (failed)'),!(x&&x.ok));loadState()})});
}
function fillForms(s){
 s.pages.forEach((p,i)=>{$('#pon'+i).checked=p.on;syncSw($('#pon'+i));$('#psec'+i).value=p.s});
 $('#gifSkip').value=s.gifSkip?'1':'0';
 $('#clk12').checked=!!s.clock12;syncSw($('#clk12'));$('#clkSec').checked=s.clockSec!==false;syncSw($('#clkSec'));
 $('#bright').value=s.bright;$('#brv').textContent=Math.round(s.bright/2.55)+'%';$('#rot').value=s.rot;
 $('#holdSw').checked=s.hold;syncSw($('#holdSw'));
 if(s.alert){const a=s.alert;$('#alOn').checked=a.on;syncSw($('#alOn'));$('#alJump').checked=a.jump;syncSw($('#alJump'));$('#alCpuT').value=a.cpuT;$('#alGpuT').value=a.gpuT;$('#alCpu').value=a.cpu;$('#alRam').value=a.ram}
 if(s.night){const n=s.night;$('#ntOn').checked=n.on;syncSw($('#ntOn'));$('#ntFrom').value=n.from;$('#ntTo').value=n.to;$('#ntBr').value=n.bright;$('#ntBrv').textContent=Math.round(n.bright/2.55)+'%'}
 $('#pcIp').value=s.pc.ip;$('#pcPort').value=s.pc.port;$('#pcPoll').value=s.pc.poll;$('#ssid').value=s.wifi.ssid}

let bt;$('#bright').addEventListener('input',e=>{$('#brv').textContent=Math.round(e.target.value/2.55)+'%';clearTimeout(bt);bt=setTimeout(()=>{post('/api/settings',{bright:+e.target.value})},120)});
$('#bright').addEventListener('change',async e=>{const r=await post('/api/settings',{bright:+e.target.value,save:true});toast('Brightness '+Math.round(e.target.value/2.55)+'%'+(r&&r.ok?' (saved)':' (failed)'),!(r&&r.ok))});
$('#rot').addEventListener('change',async e=>{const r=await post('/api/settings',{rot:+e.target.value,save:true});toast('Orientation changed',!(r&&r.ok))});
onSw($('#clk12'),'12-hour clock',v=>post('/api/settings',{clock12:v,save:true}));
onSw($('#clkSec'),'Clock seconds',v=>post('/api/settings',{clockSec:v,save:true}));
$('#gifSkip').addEventListener('change',async e=>{const r=await post('/api/settings',{gifSkip:e.target.value==='1',save:true});toast('GIF mode: '+(e.target.value==='1'?'Smooth':'All frames'),!(r&&r.ok))});

const alPost=(k,v)=>post('/api/settings',{alert:{[k]:v},save:true});
onSw($('#alOn'),'Alerts',v=>alPost('on',v));onSw($('#alJump'),'Alert jump',v=>alPost('jump',v));
[['alCpuT','cpuT',40,110],['alGpuT','gpuT',40,110],['alCpu','cpu',30,100],['alRam','ram',30,100]].forEach(([id,k,lo,hi])=>$('#'+id).addEventListener('change',async e=>{let v=Math.round(+e.target.value);if(!(v>=lo))v=lo;if(v>hi)v=hi;e.target.value=v;const r=await alPost(k,v);toast('Alert limit '+v+(r&&r.ok?' (saved)':' (failed)'),!(r&&r.ok))}));
const ntPost=(k,v)=>post('/api/settings',{night:{[k]:v},save:true});
onSw($('#ntOn'),'Night mode',v=>ntPost('on',v));
[['ntFrom','from'],['ntTo','to']].forEach(([id,k])=>$('#'+id).addEventListener('change',async e=>{let v=Math.round(+e.target.value);if(!(v>=0))v=0;if(v>23)v=23;e.target.value=v;const r=await ntPost(k,v);toast('Night '+k+' '+v+':00'+(r&&r.ok?' (saved)':' (failed)'),!(r&&r.ok))}));
$('#ntBr').addEventListener('input',e=>{$('#ntBrv').textContent=Math.round(e.target.value/2.55)+'%'});
$('#ntBr').addEventListener('change',async e=>{const r=await ntPost('bright',+e.target.value);toast('Night brightness saved',!(r&&r.ok))});
$('#bReset').onclick=async()=>{if(!confirm('Reset display settings to default? (WiFi/PC settings thakbe)'))return;const r=await post('/api/reset');toast(r&&r.ok?'Defaults restored':'Failed',!(r&&r.ok));formInit=false;loadState()};

/* ---------- system ---------- */
$('#bSavePc').onclick=async()=>{const r=await post('/api/settings',{pcIp:$('#pcIp').value.trim(),pcPort:Math.round($('#pcPort').value),pollMs:Math.round($('#pcPoll').value),save:true});toast(r&&r.ok?'PC settings saved':'Failed',!(r&&r.ok));formInit=false;loadState()};
$('#bReboot').onclick=async()=>{if(!confirm('Reboot device?'))return;await post('/api/reboot');toast('Rebooting...')};
$('#showPw').onchange=e=>{$('#pass').type=e.target.checked?'text':'password'};
function renderNets(a){const m={};a.forEach(n=>{if(!m[n.s]||m[n.s].r<n.r)m[n.s]=n});const l=Object.values(m).sort((x,y)=>y.r-x.r);
 $('#nets').innerHTML=l.length?l.map(n=>'<li><button type=button data-s="'+esc(n.s)+'"><span>'+esc(n.s)+'</span><span class=mut>'+(n.e?'secured':'open')+' &middot; '+n.r+' dBm</span></button></li>').join(''):'<li class=mut style="padding:8px">No networks found</li>';
 $$('#nets button').forEach(b=>{b.onclick=()=>{$('#ssid').value=b.dataset.s;$('#pass').focus();toast('Selected: '+b.dataset.s)}})}
async function scan(){const st=$('#scanSt'),btn=$('#bScan');st.textContent='Scanning...';btn.disabled=true;await api('/api/scan?start=1');
 for(let i=0;i<20;i++){await sleep(1000);const r=await api('/api/scan',null,true);if(r&&r.state==='done'){renderNets(r.nets||[]);st.textContent=(r.nets||[]).length+' networks found';btn.disabled=false;return}}
 st.textContent='Scan timed out';btn.disabled=false}
$('#bScan').onclick=scan;
function wmsg(t){$('#wifiMsg').textContent=t}
$('#bConn').onclick=async()=>{const ssid=$('#ssid').value.trim();if(!ssid){toast('Enter a network name',1);return}
 const r=await post('/api/wifi',{ssid:ssid,pass:$('#pass').value});if(!r||!r.ok){toast('Failed: '+((r&&r.err)||'error'),1);return}
 wmsg('Connecting to '+ssid+'... (device hotspot-o chalu thakbe)');
 for(let i=0;i<25;i++){await sleep(1000);const s=await api('/api/wifi/status',null,true);if(!s)continue;
  if(s.state==='ok'){wmsg('Connected! IP: '+s.ip+'. Phone/PC-ke oi WiFi-te niye http://'+s.ip+' ba http://'+s.mdns+' kholo.');toast('WiFi connected: '+s.ip);loadState();return}
  if(s.state==='fail'){wmsg('Could not connect. SSID/password check kore abar try koro.');toast('WiFi connection failed',1);return}}
 wmsg('Still trying... Status check korte abar page reload koro.')};
$('#bApStop').onclick=async()=>{await post('/api/ap/stop');toast('Hotspot closing...')};

/* ---------- state ---------- */
function renderSys(s){$('#sysInfo').innerHTML=[['PC hostname',s.pc.host||'-'],['ESP32 IP',s.wifi.ip],['mDNS name',s.wifi.mdns],['Signal',s.wifi.rssi+' dBm'],['Uptime',dur(s.sys.up)],['Free heap',fmtB(s.sys.heap)],['Free PSRAM',fmtB(s.sys.psram)+(s.sys.psramOk?'':' (frame buffer MISSING)')],['Firmware',s.sys.build]].map(r=>'<tr><th scope=row>'+r[0]+'</th><td>'+esc(r[1])+'</td></tr>').join('')}
function renderMedia(m){try{const pn=document.getElementById('plName');if(pn){pn.textContent=m.type==='none'?'Nothing playing':(m.name||'untitled');document.getElementById('plInfo').textContent=m.type==='none'?'':m.type+' '+m.w+'x'+m.h}}catch(e){}const t=m.type;$('#nowTxt').textContent=t==='none'?'Nothing loaded.':(m.name||'untitled')+' - '+t+' '+m.w+'x'+m.h+(m.frames>1?' ('+m.frames+' frames)':'')+', '+fmtB(m.bytes)+(m.perf&&m.perf.fps>0?' | '+m.perf.src+' '+m.perf.fps.toFixed(1)+' fps, decode '+m.perf.dec.toFixed(0)+' ms, push '+m.perf.push.toFixed(0)+' ms, dropped '+m.perf.drops:'');$('#nowTxtV').textContent=$('#nowTxt').textContent;$('#bShow').disabled=$('#bClear').disabled=$('#bShowV').disabled=$('#bClearV').disabled=(t==='none')}
async function loadState(){if(busy)return;const s=await api('/api/state',null,true);if(!s||!s.pages)return;S=s;vqHint();
 if(!formInit){buildPages(s);fillForms(s);formInit=true}
 $('#cPc').textContent='PC '+(s.pc.online?'live':'offline');$('#cPc').className='chip '+(s.pc.online?'ok':'bad');
 $('#cWifi').textContent=s.wifi.ap?'Setup hotspot':'WiFi '+s.wifi.rssi+' dBm';
 $('#cIp').textContent=s.wifi.ap&&!s.wifi.connected?'192.168.4.1':s.wifi.ip;
 $('#cUp').textContent='Up '+dur(s.sys.up);
 s.pages.forEach((p,i)=>{const b=$('#pb'+i);if(!b)return;b.setAttribute('aria-pressed',s.page===i);b.classList.toggle('off',!p.on);b.querySelector('small').textContent=p.on?p.s+' sec':'off'});
 $('#autoSw').checked=s.auto;syncSw($('#autoSw'));
 const ap=$('#apBanner');const wasHidden=ap.hidden;ap.hidden=!(s.wifi.ap&&!s.wifi.connected);if(wasHidden&&!ap.hidden)tab('sys');
 $('#bApStop').hidden=!(s.wifi.ap&&s.wifi.connected);
 renderMedia(s.media);renderSys(s);renderSd(s);if(!SDL||(s.sd&&s.sd.ok!==SDL.ok))sdRefresh()}


/* ---------- SD card manager ---------- */
let SDL=null;
const KN={1:'clip',2:'image',3:'gif',4:'jpeg',5:'avi video'};
function renderSd(s){if(!s||!s.sd)return;const d=s.sd,c=$('#sdChip');c.textContent=d.ok?'SD ready':'No SD';c.className='chip '+(d.ok?'ok':'bad');
 const used=d.total?Math.round(d.used*100/d.total):0;$('#sdBar').style.width=used+'%';
 $('#sdInfo').textContent=d.ok?(fmtB(d.total-d.used)+' free of '+fmtB(d.total)+' ('+d.mode+')'):(d.err||'Insert card (FAT32) and press Refresh');
 if(!sdFormInit){$('#sdLoops').value=d.loops;$('#sdStill').value=d.still;$('#sdPre').checked=d.preload;syncSw($('#sdPre'));sdFormInit=true}
 ['#dest','#destI'].forEach(id=>{const o=$(id);[...o.options].forEach(x=>{if(x.value!=='ram')x.disabled=!d.ok});if(!d.ok&&o.value!=='ram')o.value='ram'});
 $('#bStopAll').disabled=!d.playlist;$('#bPlayAll').disabled=!d.ok}
let sdFormInit=false;
async function sdRefresh(force){const r=await api('/api/sd/list',null,!force);if(!r)return;SDL=r;const ul=$('#sdList');ul.innerHTML='';
 if(!r.ok){ul.innerHTML='<li class=mut>'+esc(r.err||'No card')+'</li>';return}
 if(!r.files.length){ul.innerHTML='<li class=mut>No videos yet. Upload with destination "Save to SD card".</li>';return}
 r.files.sort((a,b)=>a.n.localeCompare(b.n)).forEach(f=>{const li=document.createElement('li');li.className='urow';
  const nm=document.createElement('span');nm.className='un';nm.textContent=f.n;
  const inf=document.createElement('span');inf.className='mut';inf.textContent=(KN[f.k]||'?')+(f.w?' '+f.w+'x'+f.h:'')+(f.fr?' '+f.fr+' frames':'')+' - '+fmtB(f.s);
  const pb=document.createElement('button');pb.className='btn';pb.textContent='Play';pb.setAttribute('aria-label','Play '+f.n);
  pb.onclick=async()=>{const x=await post('/api/sd/play?f='+encodeURIComponent(f.n));toast(x&&x.ok?'Playing '+f.n:'Failed: '+((x&&x.err)||'error'),!(x&&x.ok));loadState()};
  const db=document.createElement('button');db.className='btn danger';db.textContent='Delete';db.setAttribute('aria-label','Delete '+f.n);
  db.onclick=async()=>{if(!confirm('Delete '+f.n+' from SD card?'))return;const x=await post('/api/sd/delete?f='+encodeURIComponent(f.n));toast(x&&x.ok?'Deleted':'Failed',!(x&&x.ok));sdRefresh(true);loadState()};
  li.append(nm,inf,pb,db);ul.appendChild(li)})}
$('#bSdRef').onclick=async()=>{await post('/api/sd/mount');await loadState();sdRefresh(true)};
$('#bPlayAll').onclick=async()=>{const x=await post('/api/sd/playall?on=1');toast(x&&x.ok?'Playlist started':'Failed: '+((x&&x.err)||'no files'),!(x&&x.ok));loadState()};
$('#bStopAll').onclick=async()=>{await post('/api/sd/playall?on=0');toast('Playlist stopped');loadState()};
$('#sdLoops').addEventListener('change',async e=>{const r=await post('/api/settings',{sdLoops:Math.max(1,+e.target.value||1),save:true});toast(r&&r.ok?'Saved':'Failed',!(r&&r.ok))});
$('#sdStill').addEventListener('change',async e=>{const r=await post('/api/settings',{sdStill:Math.max(2,+e.target.value||8),save:true});toast(r&&r.ok?'Saved':'Failed',!(r&&r.ok))});
onSw($('#sdPre'),'Preload small clips',v=>post('/api/settings',{sdPreload:v,save:true}));
async function blobExt(b){const h=new Uint8Array(await b.slice(0,4).arrayBuffer());const m=String.fromCharCode(...h);if(m==='MJV2')return'.mjv';if(m==='RIFF')return'.avi';if(m==='RGB5')return'.rgb';if(m==='GIF8')return'.gif';return'.jpg'}
function sendSd(blob,name,row,play){return new Promise(async(ok,no)=>{
 const ext=await blobExt(blob),x=new XMLHttpRequest();
 x.open('POST','/sd/upload?name='+encodeURIComponent(name+ext)+'&size='+blob.size+'&play='+(play?1:0));
 x.upload.onprogress=e=>{if(e.lengthComputable)row.prog(e.loaded/e.total)};
 x.onload=()=>{let r={};try{r=JSON.parse(x.responseText)}catch(e){}r.ok?ok(r):no(new Error(r.err||'SD rejected'))};
 x.onerror=()=>no(new Error('network error'));x.timeout=3600000;x.ontimeout=()=>no(new Error('timeout'));
 const fd=new FormData();fd.append('f',blob,'media.bin');x.send(fd)})}

/* ---------- live stats ---------- */
const H=[];let peak=20;
function draw(){const c=$('#cv'),x=c.getContext('2d');c.width=c.clientWidth;c.height=c.clientHeight;x.clearRect(0,0,c.width,c.height);const w=c.width/120;H.forEach((v,i)=>{x.fillStyle=col(v);const h=v/100*c.height;x.fillRect(i*w,c.height-h,Math.max(1,w-1),h)})}
async function loadStats(){if(busy||$('#t-dash').hidden)return;
 const s=await api('/stats',null,true);if(!s||s.cpu===undefined)return;
 $('#cpu').textContent=Math.round(s.cpu)+'%';$('#cpu').style.color=col(s.cpu);
 $('#cpuSub').textContent=(s.name||'')+' | '+Math.round(s.freq)+' MHz';
 H.push(s.cpu);if(H.length>120)H.shift();draw();
 if(s.temp>0){$('#tc').textContent=Math.round(s.temp)+' C';$('#tc').style.color=tcol(s.temp);setBar('#tcb',s.temp,tcol(s.temp))}else{$('#tc').textContent='n/a';setBar('#tcb',0,'#777')}
 if(s.gpu_temp>0){$('#tg').textContent=Math.round(s.gpu_temp)+' C';$('#tg').style.color=tcol(s.gpu_temp);setBar('#tgb',s.gpu_temp,tcol(s.gpu_temp))}else{$('#tg').textContent='n/a';setBar('#tgb',0,'#777')}
 $('#gl').textContent=s.gpu>=0?('GPU load '+Math.round(s.gpu)+'%  |  VRAM '+s.gpu_mem_used+' / '+s.gpu_mem_total+' GB'):'GPU: no data';
 $('#ramT').textContent=Math.round(s.ram)+'%  ('+s.ram_used+' / '+s.ram_total+' GB)';setBar('#ramB',s.ram,col(s.ram));
 $('#dskT').textContent=Math.round(s.disk)+'%';setBar('#dskB',s.disk,col(s.disk));
 $('#nPing').textContent=s.ping>=0?Math.round(s.ping)+' ms':'n/a';$('#nTot').textContent=(s.rxg||0).toFixed(2)+' / '+(s.txg||0).toFixed(2)+' GB';$('#nUp').textContent=rate(s.up);$('#nDn').textContent=rate(s.down);$('#pcUp').textContent=dur(s.uptime);
 $('#nproc').textContent=s.nproc?'('+s.nproc+' total)':'';
 peak=Math.max(peak*0.97,s.dr||0,s.dw||0,20);
 $('#dRt').textContent=(s.dr||0).toFixed(1)+' MB/s';setBar('#dRb',(s.dr||0)/peak*100,'#4c9aff');
 $('#dWt').textContent=(s.dw||0).toFixed(1)+' MB/s';setBar('#dWb',(s.dw||0)/peak*100,'#f0883e');
 $('#swp').textContent=Math.round(s.swap||0)+'%';
 $('#bat').textContent=s.bat>=0?s.bat+'% '+(s.plug?'(AC)':'(battery)'):'No battery';
 $('#win').textContent=s.win||s.wapp||'(no data - Windows agent only)';$('#wapp').textContent=s.wapp?'App: '+s.wapp:'';$('#clock').textContent=(s.time||'')+'   '+(s.date||'');
 $('#drv').innerHTML=(s.drives&&s.drives.length)?s.drives.map(d=>'<div class=row><span>'+esc(d.n)+'</span><span class=sp></span><span class=mut>'+d.u+' / '+d.t+' GB &middot; '+d.p+'%</span></div><div class=meter><i style="width:'+d.p+'%;background:'+col(d.p)+'"></i></div>').join(''):'<span class=mut>No data</span>';
 $('#rps').innerHTML=(s.rprocs&&s.rprocs.length)?s.rprocs.map(p=>'<tr><td>'+esc(p.n)+'</td><td class=mut>'+(p.m>=1024?(p.m/1024).toFixed(1)+' GB':p.m+' MB')+'</td></tr>').join(''):'<tr><td class=mut>No data</td></tr>';
 $('#dps').innerHTML=(s.dprocs&&s.dprocs.length)?s.dprocs.map(p=>'<tr><td>'+esc(p.n)+'</td><td style="color:#e0a82e">'+p.r.toFixed(1)+' MB/s</td></tr>').join(''):'<tr><td class=mut>Idle</td></tr>';
 $('#procs').innerHTML=(s.procs&&s.procs.length)?s.procs.map(p=>'<tr><td>'+esc(p.n)+'</td><td style="color:'+col(p.c)+'">'+p.c.toFixed(1)+'%</td><td class=mut>'+(p.m>=1024?(p.m/1024).toFixed(1)+' GB':p.m+' MB')+'</td></tr>').join(''):'<tr><td class=mut colspan=3>No data (pc_agent.py cholche?)</td></tr>'}

/* ---------- media: convert in browser, send to RAM, show instantly ---------- */
const showMedia=()=>{api('/api/page?n='+(S?S.pages.length-1:5)).then(()=>{toast('Showing media');loadState()})};$('#bShow').onclick=$('#bShowV').onclick=showMedia;
$('#bClear').onclick=$('#bClearV').onclick=async()=>{if(!confirm('Clear media from device RAM?'))return;const r=await post('/api/media/clear');toast(r&&r.ok?'Media cleared':'Failed',!(r&&r.ok));loadState()};
const tb=(c,q)=>new Promise((ok,no)=>{c.toBlob(b=>b?ok(b):no(new Error('JPEG encode failed')),'image/jpeg',q)});
let BUDGET=5300000;
/* ---------- pure helpers: GIF decode, delta boxes, MJV2 pack ---------- */
function lzwDecode(data,minCode,total){
 const clear=1<<minCode,eoi=clear+1;let size=minCode+1,mask=(1<<size)-1,next=eoi+1;
 const prefix=new Uint16Array(4096),suffix=new Uint8Array(4096),stack=new Uint8Array(4097);
 const out=new Uint8Array(total);let o=0,bits=0,acc=0,di=0,prev=-1,first=0;
 for(let i=0;i<clear;i++)suffix[i]=i;
 while(o<total){
  while(bits<size){if(di>=data.length)return out;acc|=data[di++]<<bits;bits+=8}
  const code=acc&mask;acc>>=size;bits-=size;
  if(code===clear){size=minCode+1;mask=(1<<size)-1;next=eoi+1;prev=-1;continue}
  if(code===eoi)break;
  if(prev===-1){out[o++]=suffix[code];prev=code;first=suffix[code];continue}
  let sp=0,cur=code;
  if(code>=next){stack[sp++]=first;cur=prev}
  while(cur>=clear){stack[sp++]=suffix[cur];cur=prefix[cur]}
  first=suffix[cur];stack[sp++]=first;
  while(sp>0&&o<total)out[o++]=stack[--sp];
  if(next<4096){prefix[next]=prev;suffix[next]=first;next++;if(next===(1<<size)&&size<12){size++;mask=(1<<size)-1}}
  prev=code}
 return out}
function interlaceRows(h){const r=[];for(let y=0;y<h;y+=8)r.push(y);for(let y=4;y<h;y+=8)r.push(y);for(let y=2;y<h;y+=4)r.push(y);for(let y=1;y<h;y+=2)r.push(y);return r}
function clearRect(cv,W,H,r){const x0=Math.max(0,r[0]),y0=Math.max(0,r[1]),x1=Math.min(W,r[0]+r[2]),y1=Math.min(H,r[1]+r[3]);if(x1<=x0)return;for(let y=y0;y<y1;y++)cv.fill(0,(y*W+x0)*4,(y*W+x1)*4)}
function* gifFrames(buf){
 const u=new Uint8Array(buf);
 if(u.length<13||u[0]!==71||u[1]!==73||u[2]!==70)throw new Error('Not a GIF file');
 const W=u[6]|(u[7]<<8),H=u[8]|(u[9]<<8),fl=u[10];let p=13,gct=null;
 if(fl&0x80){const n=3<<((fl&7)+1);gct=u.subarray(p,p+n);p+=n}
 const cv=new Uint8ClampedArray(W*H*4);let gce=null,prevDisp=0,prevRect=null,snap=null;
 while(p<u.length){
  const b=u[p++];
  if(b===0x3B)break;
  if(b===0x21){const lab=u[p++];
   if(lab===0xF9){const pk=u[p+1];gce={disp:(pk>>2)&7,tr:(pk&1)?u[p+4]:-1,delay:(u[p+2]|(u[p+3]<<8))*10}}
   while(p<u.length&&u[p]!==0)p+=u[p]+1;p++;continue}
  if(b!==0x2C)continue;
  const fx=u[p]|(u[p+1]<<8),fy=u[p+2]|(u[p+3]<<8),fw=u[p+4]|(u[p+5]<<8),fh=u[p+6]|(u[p+7]<<8),pk=u[p+8];p+=9;
  let ct=gct;if(pk&0x80){const n=3<<((pk&7)+1);ct=u.subarray(p,p+n);p+=n}
  const minCode=u[p++];
  const chunks=[];let len=0;
  while(p<u.length&&u[p]!==0){const s=u[p];chunks.push(u.subarray(p+1,p+1+s));len+=s;p+=s+1}p++;
  const data=new Uint8Array(len);let o=0;for(const c of chunks){data.set(c,o);o+=c.length}
  const idx=lzwDecode(data,minCode,fw*fh);
  if(prevDisp===2&&prevRect)clearRect(cv,W,H,prevRect);else if(prevDisp===3&&snap)cv.set(snap);
  const disp=gce?gce.disp:0,tr=gce?gce.tr:-1;
  if(disp===3)snap=cv.slice();
  const rows=(pk&0x40)?interlaceRows(fh):null;
  if(ct){for(let r=0;r<fh;r++){const yy=fy+(rows?rows[r]:r);if(yy>=H)continue;
   for(let c=0;c<fw;c++){const xx=fx+c;if(xx>=W)continue;const ci=idx[r*fw+c];if(ci===tr)continue;const q=(yy*W+xx)*4,k=ci*3;cv[q]=ct[k];cv[q+1]=ct[k+1];cv[q+2]=ct[k+2];cv[q+3]=255}}}
  yield{w:W,h:H,delay:gce?gce.delay:0,rgba:cv};
  prevDisp=disp;prevRect=[fx,fy,fw,fh];gce=null}
}
function diffBox(a,b,w,h,thr){let x0=w,y0=h,x1=-1,y1=-1;
 for(let y=0;y<h;y++){const r=y*w*4;let hit=false;
  for(let x=0;x<w;x++){const i=r+x*4;const d=Math.abs(a[i]-b[i])+Math.abs(a[i+1]-b[i+1])+Math.abs(a[i+2]-b[i+2]);
   if(d>thr){if(x<x0)x0=x;if(x>x1)x1=x;hit=true}}
  if(hit){if(y<y0)y0=y;y1=y}}
 return x1<0?null:[x0,y0,x1,y1]}
function alignBox(bx,w,h,pad){const x0=Math.max(0,(bx[0]-pad)&~1),y0=Math.max(0,(bx[1]-pad)&~1);
 const x1=Math.min(w,(bx[2]+pad+2)&~1),y1=Math.min(h,(bx[3]+pad+2)&~1);return[x0,y0,x1-x0,y1-y0]}
function packMjv2(ents,w,h,delay,allKey){const n=ents.length;let total=16+12*n;ents.forEach(e=>{total+=e.data?e.data.length:0});
 const buf=new ArrayBuffer(total),dv=new DataView(buf),u8=new Uint8Array(buf);
 u8.set([77,74,86,50],0);dv.setUint16(4,w,true);dv.setUint16(6,h,true);dv.setUint16(8,delay,true);dv.setUint16(10,allKey?1:0,true);dv.setUint32(12,n,true);
 let t=16,o=16+12*n;
 ents.forEach(e=>{dv.setUint16(t,e.x,true);dv.setUint16(t+2,e.y,true);dv.setUint16(t+4,e.w,true);dv.setUint16(t+6,e.h,true);const s=e.data?e.data.length:0;dv.setUint32(t+8,s,true);t+=12;if(s){u8.set(e.data,o);o+=s}});
 return buf}

function preset(){const m={best:{w:320,h:240,fps:10,q:.90},full:{w:320,h:240,fps:12,q:.82},smooth:{w:320,h:214,fps:12,q:.82},balanced:{w:280,h:210,fps:15,q:.82},fast:{w:240,h:160,fps:20,q:.82},wide:{w:320,h:180,fps:15,q:.82},full18:{w:160,h:120,fps:25,q:.86}}[$('#vq').value];return Object.assign({maxSec:Math.max(1,Math.min(3600,+$('#vmax').value||15)),thr:30},m)}
function vqHint(){const e=$('#vqHint');if(e)e.textContent='CYD: video SD card-e save kore play hoy, max 320x240.'}
$('#vq').addEventListener('change',vqHint);
function fitSize(vw,vh,P){const s=Math.min(P.w/vw,P.h/vh);return[Math.max(8,Math.round(vw*s)&~1),Math.max(8,Math.round(vh*s)&~1)]}

async function imgToRgb5(file,mode){
 const bmp=await createImageBitmap(file);const W=320,H=240;
 let cw=W,ch=H,dx=0,dy=0,dw=W,dh=H,sx=0,sy=0,sw=bmp.width,sh=bmp.height;
 if(mode==='contain'){const s=Math.min(W/bmp.width,H/bmp.height);dw=Math.max(1,Math.round(bmp.width*s));dh=Math.max(1,Math.round(bmp.height*s));cw=dw;ch=dh}
 else if(mode==='cover'){const s=Math.max(W/bmp.width,H/bmp.height);sw=Math.round(W/s);sh=Math.round(H/s);sx=Math.round((bmp.width-sw)/2);sy=Math.round((bmp.height-sh)/2)}
 const c=document.createElement('canvas');c.width=cw;c.height=ch;const x=c.getContext('2d');x.imageSmoothingQuality='high';x.fillStyle='#000';x.fillRect(0,0,cw,ch);
 x.drawImage(bmp,sx,sy,sw,sh,dx,dy,dw,dh);if(bmp.close)bmp.close();
 const px=x.getImageData(0,0,cw,ch).data;const out=new ArrayBuffer(8+cw*ch*2),dv=new DataView(out);
 new Uint8Array(out).set([82,71,66,53],0);dv.setUint16(4,cw,true);dv.setUint16(6,ch,true);
 /* Floyd-Steinberg dithering to RGB565: no colour banding (much nicer gradients / skin / sky) */
 const f=new Float32Array(cw*ch*3);for(let i=0,j=0;i<px.length;i+=4,j+=3){f[j]=px[i];f[j+1]=px[i+1];f[j+2]=px[i+2]}
 let o=8;const sp=(j,v)=>{if(j>=0&&j<f.length)f[j]+=v};
 for(let y=0;y<ch;y++)for(let xx=0;xx<cw;xx++){const j=(y*cw+xx)*3;
  const r=Math.max(0,Math.min(255,f[j])),g=Math.max(0,Math.min(255,f[j+1])),b=Math.max(0,Math.min(255,f[j+2]));
  const rq=Math.round(r/255*31),gq=Math.round(g/255*63),bq=Math.round(b/255*31);
  const er=r-((rq<<3)|(rq>>2)),eg=g-((gq<<2)|(gq>>4)),eb=b-((bq<<3)|(bq>>2));
  const R=xx<cw-1,L=xx>0,D=y<ch-1;
  for(let k=0;k<3;k++){const e=k===0?er:k===1?eg:eb;
   if(R)sp(j+3+k,e*7/16);
   if(D){if(L)sp(j+cw*3-3+k,e*3/16);sp(j+cw*3+k,e*5/16);if(R)sp(j+cw*3+3+k,e/16)}}
  dv.setUint16(o,(rq<<11)|(gq<<5)|bq,true);o+=2}
 return new Blob([out])}

/* ---- AVI (MJPEG + PCM u8 audio) muxer: sound comes with the picture, the CYD keeps them in sync ---- */
function packAvi(ents,w,h,fps,aud){
 const n=ents.length,usec=Math.round(1e6/fps),rate=aud?aud.rate:0;
 const u32=v=>{const b=new Uint8Array(4);new DataView(b.buffer).setUint32(0,v>>>0,true);return b};
 const u16=v=>{const b=new Uint8Array(2);new DataView(b.buffer).setUint16(0,v&65535,true);return b};
 const cc=s=>new Uint8Array([...s].map(c=>c.charCodeAt(0)));
 const cat=(...a)=>{let l=0;a.forEach(x=>l+=x.length);const o=new Uint8Array(l);let p=0;a.forEach(x=>{o.set(x,p);p+=x.length});return o};
 const E=new Uint8Array(0);
 const chunk=(id,d)=>cat(cc(id),u32(d.length),d,d.length&1?new Uint8Array(1):E);
 const list=(t,...parts)=>{const b=cat(cc(t),...parts);return cat(cc('LIST'),u32(b.length),b)};
 let maxF=0;ents.forEach(e=>{if(e.data.length>maxF)maxF=e.data.length});
 const avih=cat(u32(usec),u32(Math.round(maxF*fps)),u32(0),u32(0x10),u32(n),u32(0),u32(aud?2:1),u32(maxF),u32(w),u32(h),u32(0),u32(0),u32(0),u32(0));
 const strhV=cat(cc('vids'),cc('MJPG'),u32(0),u16(0),u16(0),u32(0),u32(1),u32(fps),u32(0),u32(n),u32(maxF),u32(0xFFFFFFFF),u32(0),u16(0),u16(0),u16(w),u16(h));
 const strfV=cat(u32(40),u32(w),u32(h),u16(1),u16(24),cc('MJPG'),u32(w*h*3),u32(0),u32(0),u32(0),u32(0));
 let hdrl=[chunk('avih',avih),list('strl',chunk('strh',strhV),chunk('strf',strfV))];
 if(aud){
  const strhA=cat(cc('auds'),u32(0),u32(0),u16(0),u16(0),u32(0),u32(1),u32(rate),u32(0),u32(aud.pcm.length),u32(4096),u32(0xFFFFFFFF),u32(1),u16(0),u16(0),u16(0),u16(0));
  const strfA=cat(u16(1),u16(1),u32(rate),u32(rate),u16(1),u16(8),u16(0));
  hdrl.push(list('strl',chunk('strh',strhA),chunk('strf',strfA)))}
 const H=list('hdrl',...hdrl);
 const mv=[],idx=[];let pos=4;
 const add=(id,d)=>{const c=chunk(id,d);mv.push(c);idx.push(cat(cc(id),u32(0x10),u32(pos),u32(d.length)));pos+=c.length};
 for(let j=0;j<n;j++){
  add('00dc',ents[j].data);
  if(aud){const a=Math.round(j*rate/fps),b=j===n-1?aud.pcm.length:Math.round((j+1)*rate/fps);if(b>a)add('01wb',aud.pcm.subarray(a,Math.min(b,aud.pcm.length)))}}
 const M=list('movi',...mv),I=chunk('idx1',cat(...idx));
 return cat(cc('RIFF'),u32(4+H.length+M.length+I.length),cc('AVI '),H,M,I)}

/* browser decodes the audio track -> mono, 22.05 kHz, 8-bit unsigned PCM */
async function extractAudio(file,maxSec){
 try{
  const AC=window.AudioContext||window.webkitAudioContext;if(!AC)return null;
  const ac=new AC({sampleRate:22050});
  const buf=await ac.decodeAudioData(await file.arrayBuffer());ac.close();
  const n=Math.min(buf.length,Math.floor(maxSec*22050)),chs=[];
  for(let c=0;c<buf.numberOfChannels;c++)chs.push(buf.getChannelData(c));
  const m=new Float32Array(n);let pk=0.001;
  for(let i=0;i<n;i++){let s=0;for(let c=0;c<chs.length;c++)s+=chs[c][i];s/=chs.length;m[i]=s;const a=Math.abs(s);if(a>pk)pk=a}
  const g=Math.min(3,0.92/pk),out=new Uint8Array(n);
  for(let i=0;i<n;i++){const v=Math.round(m[i]*g*127+128);out[i]=v<0?0:v>255?255:v}
  return n>2000?out:null}catch(e){return null}}

/* frames -> MJV2 clip. allowDelta: sudhu bodlano tile pathay (GIF/animation), na hole full frame (video, skip-able) */
async function clipBuild(w,h,P,total,drawFrame,prog,allowDelta,aud){
 const out=document.createElement('canvas');out.width=w;out.height=h;
 const ox=out.getContext('2d',{willReadFrequently:true});ox.imageSmoothingQuality='high';
 const tmp=document.createElement('canvas'),tx=tmp.getContext('2d');
 const ents=[];let sent=null,bytes=0,nDelta=0;
 for(let j=0;j<total;j++){
  await drawFrame(j,ox,w,h);
  const cur=ox.getImageData(0,0,w,h).data;
  let rect=[0,0,w,h];
  if(sent&&allowDelta){
   const b=diffBox(sent,cur,w,h,P.thr);
   if(!b){ents.push({x:0,y:0,w:0,h:0,data:null});prog((j+1)/total);continue}
   const r=alignBox(b,w,h,4);
   if(r[2]*r[3]<=0.7*w*h){rect=r;nDelta++}
  }
  tmp.width=rect[2];tmp.height=rect[3];
  tx.drawImage(out,rect[0],rect[1],rect[2],rect[3],0,0,rect[2],rect[3]);
  const u8=new Uint8Array(await(await tb(tmp,P.q)).arrayBuffer());
  if(bytes+u8.length>BUDGET)break;
  bytes+=u8.length;
  ents.push({x:rect[0],y:rect[1],w:rect[2],h:rect[3],data:u8});
  if(!sent||(rect[2]===w&&rect[3]===h))sent=new Uint8ClampedArray(cur);
  else{for(let y=rect[1];y<rect[1]+rect[3];y++){const a=(y*w+rect[0])*4;sent.set(cur.subarray(a,a+rect[2]*4),a)}}
  prog((j+1)/total)}
 if(!ents.length)throw new Error('No frames extracted');
 if(aud&&!allowDelta)return new Blob([packAvi(ents,w,h,P.fps,aud)]);
 return new Blob([packMjv2(ents,w,h,Math.round(1000/P.fps),nDelta===0)])}

async function videoToClip(file,P,prog,wantAudio){
 const url=URL.createObjectURL(file);
 try{
  const v=document.createElement('video');v.muted=true;v.playsInline=true;v.preload='auto';v.src=url;
  await new Promise((ok,no)=>{v.onloadeddata=ok;v.onerror=()=>no(new Error('Browser cannot decode this video'))});
  if(!isFinite(v.duration)||v.duration<=0)throw new Error('Unknown video length');
  const dur=Math.min(v.duration,P.maxSec),[w,h]=fitSize(v.videoWidth,v.videoHeight,P),n=Math.max(1,Math.floor(dur*P.fps));
  const draw=async(j,ox)=>{v.currentTime=Math.min((j+0.5)/P.fps,Math.max(0,v.duration-0.05));await Promise.race([new Promise(r=>{v.onseeked=r}),sleep(4000)]);ox.drawImage(v,0,0,w,h)};
  let aud=null;
  if(wantAudio){aud=await extractAudio(file,dur);if(!aud)toast('Is video-te sound paoa gelo na (video-only banailam)',1)}
  return await clipBuild(w,h,P,n,draw,prog,false,aud?{pcm:aud,rate:22050}:null)
 }finally{URL.revokeObjectURL(url)}}

async function gifToClip(file,P,prog){
 const buf=await file.arrayBuffer();
 let W=0,H=0;const durs=[];
 for(const f of gifFrames(buf)){W=f.w;H=f.h;durs.push(Math.max(20,f.delay||100))}
 if(!durs.length)throw new Error('Empty GIF');
 const cum=[];let t=0;durs.forEach(d=>{t+=d;cum.push(t)});
 const total=Math.max(1,Math.floor(Math.min(t,P.maxSec*1000)*P.fps/1000)),[w,h]=fitSize(W,H,P);
 const src=document.createElement('canvas');src.width=W;src.height=H;const sx=src.getContext('2d');
 const it=gifFrames(buf);let cur=it.next(),fi=0,drawn=-1;
 const draw=async(j,ox)=>{const tj=j*1000/P.fps;
  while(fi<cum.length-1&&cum[fi]<=tj){cur=it.next();fi++}
  if(drawn!==fi){sx.putImageData(new ImageData(cur.value.rgba,W,H),0,0);drawn=fi}
  ox.fillStyle='#000';ox.fillRect(0,0,w,h);ox.drawImage(src,0,0,w,h)};
 return await clipBuild(w,h,P,total,draw,prog,true)}

async function decoderToClip(file,P,prog){
 const dec=new ImageDecoder({data:await file.arrayBuffer(),type:file.type});await dec.tracks.ready;
 const n=dec.tracks.selectedTrack.frameCount,durs=[];let W=0,H=0;
 for(let i=0;i<n;i++){const r=await dec.decode({frameIndex:i});if(!i){W=r.image.displayWidth;H=r.image.displayHeight}durs.push(Math.max(20,r.image.duration?r.image.duration/1000:100));r.image.close()}
 const cum=[];let t=0;durs.forEach(d=>{t+=d;cum.push(t)});
 const total=Math.max(1,Math.floor(Math.min(t,P.maxSec*1000)*P.fps/1000)),[w,h]=fitSize(W,H,P);let last=-1;
 const draw=async(j,ox)=>{const tj=j*1000/P.fps;let idx=cum.findIndex(q=>q>tj);if(idx<0)idx=n-1;
  if(idx!==last){const r=await dec.decode({frameIndex:idx});ox.fillStyle='#000';ox.fillRect(0,0,w,h);ox.drawImage(r.image,0,0,w,h);r.image.close();last=idx}};
 try{return await clipBuild(w,h,P,total,draw,prog,true)}finally{dec.close()}}

function addRow(name,kind){const li=document.createElement('li');li.className='urow';
 li.innerHTML='<span class=un></span><div class=pb role=progressbar aria-valuemin=0 aria-valuemax=100 aria-valuenow=0><i></i></div><span class=ust></span>';
 li.querySelector('.un').textContent=name;(kind==='image'?$('#uplI'):$('#upl')).prepend(li);
 const bar=li.querySelector('.pb'),fill=bar.firstChild,st=li.querySelector('.ust');
 return{msg:t=>{st.textContent=t;st.style.color=''},prog:f=>{const p=Math.round(f*100);fill.style.width=p+'%';bar.setAttribute('aria-valuenow',p)},done:t=>{fill.style.width='100%';st.textContent=t;st.style.color='var(--ok)'},fail:t=>{st.textContent=t;st.style.color='var(--bad)'}}}

function sendLive(blob,name,row){return new Promise((ok,no)=>{
 const hold=$('#holdSw').checked?1:0,x=new XMLHttpRequest();
 x.open('POST','/live?size='+blob.size+'&hold='+hold+'&name='+encodeURIComponent(name));
 x.upload.onprogress=e=>{if(e.lengthComputable)row.prog(e.loaded/e.total)};
 x.onload=()=>{let r={};try{r=JSON.parse(x.responseText)}catch(e){}r.ok?ok(r):no(new Error(r.err||'rejected by device'))};
 x.onerror=()=>no(new Error('network error'));x.timeout=180000;x.ontimeout=()=>no(new Error('timeout'));
 const fd=new FormData();fd.append('f',blob,'media.bin');x.send(fd)})}

async function processFile(f,row,kind){
 const t=f.type||'',ext=(f.name.split('.').pop()||'').toLowerCase(),base=f.name.replace(/\.[^.]+$/,'').slice(0,40)||'media',P=preset();
 const isVid=t.startsWith('video/')||/^(mp4|webm|mov|m4v|mkv|ogv|avi)$/.test(ext);
 const isGif=t==='image/gif'||ext==='gif';
 const clipMode=$('#animMode').value==='clip',dest0=(kind==='image'?$('#destI'):$('#dest')).value,dest=(dest0==='ram'&&S&&S.sys&&S.sys.psramOk===false)?'sdplay':dest0;
 BUDGET=dest==='ram'?5300000:Math.max(5300000,Math.min(150000000,((S&&S.sd?S.sd.total-S.sd.used:0)-2097152)));
 let blob;
 if(isVid){row.msg('Converting video...');blob=await videoToClip(f,P,row.prog,$('#audioOn')&&$('#audioOn').checked)}
 else if(isGif&&clipMode){
  try{row.msg('Converting GIF...');blob=await gifToClip(f,P,row.prog)}
  catch(e){if(f.size>3145728)throw e;row.msg('Clip convert failed, sending original GIF...');blob=f}}
 else if(isGif){if(f.size>3145728)throw new Error('Original GIF >3MB: use smooth clip mode');blob=f}
 else{
  let anim=false;
  if(clipMode&&window.ImageDecoder&&/^image\/(webp|apng|png|avif)$/.test(t)){try{const d=new ImageDecoder({data:await f.arrayBuffer(),type:t});await d.tracks.ready;anim=d.tracks.selectedTrack.frameCount>1;d.close()}catch(e){anim=false}}
  if(anim){row.msg('Converting animation...');blob=await decoderToClip(f,P,row.prog)}
  else{row.msg('Converting image...');blob=await imgToRgb5(f,$('#fit').value)}
 }
 row.msg('Sending '+fmtB(blob.size)+'...');row.prog(0);
 if(dest==='ram'){await sendLive(blob,base,row);row.done('Showing now');toast('Now showing: '+base)}
 else{await sendSd(blob,base,row,dest==='sdplay');row.done(dest==='sdplay'?'Saved to SD, playing':'Saved to SD');toast('Saved to SD: '+base);sdRefresh(true)}}

async function handleFiles(list,kind){const files=[...list];if(!files.length)return;if(busy){toast('Upload already running',1);return}
 busy=true;
 for(const f of files){const row=addRow(f.name,kind);try{await processFile(f,row,kind)}catch(e){row.fail((e&&e.message)||'failed');toast('Failed: '+((e&&e.message)||'error'),1)}}
 busy=false;loadState()}
function wireDz(dz,fi,kind){
 dz.onclick=()=>fi.click();
 fi.onchange=()=>{handleFiles(fi.files,kind);fi.value=''};
 ['dragover','dragenter'].forEach(e=>dz.addEventListener(e,ev=>{ev.preventDefault();dz.classList.add('hot')}));
 ['dragleave','drop'].forEach(e=>dz.addEventListener(e,ev=>{ev.preventDefault();dz.classList.remove('hot')}));
 dz.addEventListener('drop',ev=>handleFiles(ev.dataTransfer.files,kind))}
wireDz($('#dzI'),$('#fiI'),'image');wireDz($('#dz'),$('#fi'),'video');
document.addEventListener('paste',e=>{const k=!$('#t-image').hidden?'image':(!$('#t-video').hidden?'video':null);if(!k)return;const fs=[...(e.clipboardData&&e.clipboardData.files||[])];if(fs.length)handleFiles(fs,k)});

/* ---------- start ---------- */
{let h=location.hash.slice(1);if(h==='media')h='image';if(TABS.indexOf(h)>=0)tab(h)}
vqHint();
loadState();loadStats();
setInterval(loadState,3000);setInterval(loadStats,1000);

/* ===== appearance, device theme, volume, player, keyboard shortcuts ===== */
(function(){
 const root=document.documentElement;
 const sel=$('#wtheme');
 try{sel.value=localStorage.wt||'auto'}catch(e){}
 sel.addEventListener('change',()=>{const v=sel.value;try{localStorage.wt=v}catch(e){}
  if(v==='auto')delete root.dataset.theme;else root.dataset.theme=v;toast('Web theme: '+sel.options[sel.selectedIndex].text)});
 let fs=1;try{fs=parseFloat(localStorage.wfs)||1}catch(e){}
 const setFs=v=>{fs=Math.max(.85,Math.min(1.6,Math.round(v*100)/100));root.style.setProperty('--fs',fs);try{localStorage.wfs=fs}catch(e){}toast('Text size '+Math.round(fs*100)+'%')};
 $('#fsm').addEventListener('click',()=>setFs(fs-.1));
 $('#fsp').addEventListener('click',()=>setFs(fs+.1));

 const dt=$('#devTheme'),vol=$('#vol'),volv=$('#volv');let vt=null,built=false;
 const ff=fillForms;
 fillForms=function(s){ff(s);
  if(s.themes&&!built){dt.innerHTML='';s.themes.forEach((n,i)=>{const o=document.createElement('option');o.value=i;o.textContent=n;dt.appendChild(o)});built=true}
  if(typeof s.theme==='number')dt.value=s.theme;
  if(typeof s.vol==='number'&&document.activeElement!==vol){vol.value=s.vol;volv.textContent=s.vol+'%'}
  if(s.audio===false){vol.disabled=true;$('#volHint').textContent='Ei board-e audio output nei.'}};
 dt.addEventListener('change',async()=>{const r=await post('/api/settings',{theme:+dt.value});toast('Device theme: '+dt.options[dt.selectedIndex].text,!(r&&r.ok))});
 vol.addEventListener('input',()=>{volv.textContent=vol.value+'%';clearTimeout(vt);vt=setTimeout(()=>post('/api/settings',{vol:+vol.value}),180)});
 const step=async d=>{const r=await post('/api/media/step?d='+d);toast(d>0?'Next media':'Previous media',!(r&&r.ok));setTimeout(loadState,600)};
 const page=async d=>{await api(d>0?'/api/next':'/api/prev');setTimeout(loadState,300)};
 $('#bPrevM').addEventListener('click',()=>step(-1));
 $('#bNextM').addEventListener('click',()=>step(1));
 $('#bPrevP').addEventListener('click',()=>page(-1));
 $('#bNextP').addEventListener('click',()=>page(1));
 const setVol=d=>{const v=Math.max(0,Math.min(100,(+vol.value)+d));vol.value=v;volv.textContent=v+'%';post('/api/settings',{vol:v});toast('Volume '+v+'%')};
 document.addEventListener('keydown',e=>{
  if(!e.altKey||e.ctrlKey||e.metaKey)return;
  const k=e.key;
  if(k===','||k==='<'){e.preventDefault();step(-1)}
  else if(k==='.'||k==='>'){e.preventDefault();step(1)}
  else if(k==='['){e.preventDefault();page(-1)}
  else if(k===']'){e.preventDefault();page(1)}
  else if(k==='ArrowUp'){e.preventDefault();setVol(6)}
  else if(k==='ArrowDown'){e.preventDefault();setVol(-6)}});
})();

/* ===== PC remote editor: tabs + 10 buttons per tab ===== */
(function(){
 const G={Media:'playpause next prev stop mute volup voldown',
  Windows:'lock desktop alttab taskview taskmgr screenshot minimize maximize snap_left snap_right run explorer settings search clipboard emoji newdesk nextdesk prevdesk gamebar project notify voicetype',
  Power:'monitor_off sleep restart shutdown cancel_shutdown closewin',
  Edit:'copy cut paste undo redo selectall save find',
  Browser:'newtab closetab reopentab nexttab prevtab refresh fullscreen back forward zoomin zoomout zoomreset addrbar incognito',
  Keys:'up down left right esc enter space tab backspace delete pgup pgdn home end',
  Slides:'slide_start slide_here blackscreen'};
 const PH={'keys:':'ctrl+shift+n','url:':'https://youtube.com','app:':'notepad'};
 const KL={'keys:':'Shortcut (e.g. ctrl+shift+n)','url:':'Link (http/https)','app:':'App / file / folder'};
 let acts=[],tabs=[],cur=0,sel=-1,max=8,per=10,dirty=false,loaded=false,tt=0;
 const $$=id=>document.getElementById(id);
 const aid=id=>acts.findIndex(a=>a.id===id);
 const kindOf=id=>aid(id)>=0?id:(['url:','app:'].find(p=>id.startsWith(p))||'keys:');
 const slotUser=s=>s<8?s:(s===9?8:(s===10?9:-1));
 const userSlot=u=>u<8?u:u+1;
 function mark(d){dirty=d;$$('rmState').innerHTML=d?'<span class=rm-dirty>&#9679; Save kora hoyni</span>':'Saved';$$('rmSave').disabled=!d;$$('rmRevert').disabled=!d}
 function say(m,bad){const t=$$('rmToast');t.textContent=m;t.style.borderColor=bad?'#ff4d4d':'#2ecc71';t.style.color=bad?'#ff9a9a':'#bde8c8'}
 function fillActs(){const s=$$('rmAct');s.innerHTML='';
  const used=new Set();
  Object.keys(G).forEach(g=>{const og=document.createElement('optgroup');og.label=g;
   G[g].split(' ').forEach(id=>{const k=aid(id);if(k<0)return;used.add(id);const o=document.createElement('option');o.value=id;o.textContent=acts[k].l+(acts[k].d?' (confirm)':'');og.appendChild(o)});
   s.appendChild(og)});
  const rest=acts.filter(a=>!used.has(a.id));
  if(rest.length){const og=document.createElement('optgroup');og.label='Other';rest.forEach(a=>{const o=document.createElement('option');o.value=a.id;o.textContent=a.l;og.appendChild(o)});s.appendChild(og)}
  const og=document.createElement('optgroup');og.label='Custom';
  [['keys:','Custom shortcut...'],['url:','Open link...'],['app:','Launch app / file...']].forEach(x=>{const o=document.createElement('option');o.value=x[0];o.textContent=x[1];og.appendChild(o)});
  s.appendChild(og)}
 function renderTabs(){const box=$$('rmTabs');box.innerHTML='';
  tabs.forEach((t,i)=>{const b=document.createElement('button');b.type=='button';b.type='button';b.className='rm-tab';b.textContent=t.n||'TAB';b.setAttribute('aria-pressed',i===cur);b.onclick=()=>{cur=i;sel=-1;render()};box.appendChild(b)});
  if(tabs.length<max){const b=document.createElement('button');b.type='button';b.className='rm-tab add';b.textContent='+ Tab';b.setAttribute('aria-label','Add a tab');
   b.onclick=()=>{tabs.push({n:'TAB '+(tabs.length+1),b:[]});cur=tabs.length-1;sel=-1;render();mark(true);$$('rmName').focus();$$('rmName').select()};box.appendChild(b)}}
 function renderGrid(){const g=$$('rmGrid');g.innerHTML='';const t=tabs[cur];
  $$('rmTabName').innerHTML=t?(t.n+'<small>'+(cur+1)+'/'+tabs.length+'</small>'):'-';
  $$('rmPrevT').disabled=$$('rmNextT').disabled=tabs.length<2;
  for(let s=0;s<12;s++){const b=document.createElement('button');b.type='button';b.className='rm-cell';
   if(s===8||s===11){b.className+=' nav';b.disabled=true;
    if(tabs.length>1){const n=tabs[(cur+(s===11?1:tabs.length-1))%tabs.length].n;b.innerHTML=(s===11?'NEXT TAB &gt;':'&lt; PREV TAB')+'<b></b>';b.querySelector('b').textContent=n}
    b.setAttribute('aria-label',(s===11?'Next':'Previous')+' tab button (automatic)');g.appendChild(b);continue}
   const u=slotUser(s);const x=t&&t.b[u];
   if(x){b.textContent=x.l;if(x.d)b.className+=' danger';b.setAttribute('aria-label','Button '+(u+1)+': '+x.l+'. Edit');b.setAttribute('aria-pressed',u===sel)}
   else{b.className+=' empty';b.textContent='+';b.setAttribute('aria-label','Add button at position '+(u+1));
    if(!t||u!==t.b.length){b.textContent='';b.onclick=()=>say('Age ager slot-gulo bhoro',true);b.setAttribute('aria-disabled','true')}
    else b.onclick=()=>addBtn()}
   if(x)b.onclick=()=>{sel=u;render();$$('rmAct').focus()};
   g.appendChild(b)}}
 function renderIns(){const t=tabs[cur];$$('rmTabEd').hidden=!t;if(t&&document.activeElement!==$$('rmName'))$$('rmName').value=t.n;
  $$('rmTabL').disabled=cur===0;$$('rmTabR').disabled=cur>=tabs.length-1;$$('rmTabDel').disabled=tabs.length<=1;
  const x=t&&t.b[sel];$$('rmBtnForm').hidden=!x;$$('rmNone').hidden=!!x;$$('rmBtnTitle').textContent=x?'Button '+(sel+1)+' (position '+(userSlot(sel)+1)+')':'Button';
  if(!x)return;const k=kindOf(x.id);$$('rmAct').value=k;
  const c=!(aid(x.id)>=0);$$('rmCustomW').hidden=!c;
  if(c){$$('rmCustomL').textContent=KL[k];$$('rmCustom').placeholder=PH[k];if(document.activeElement!==$$('rmCustom'))$$('rmCustom').value=x.id.slice(k.length)}
  if(document.activeElement!==$$('rmLabel'))$$('rmLabel').value=x.l;$$('rmConfirm').checked=!!x.d;
  $$('rmL').disabled=sel===0;$$('rmR').disabled=sel>=t.b.length-1}
 function render(){if(cur>=tabs.length)cur=tabs.length-1;if(cur<0)cur=0;renderTabs();renderGrid();renderIns()}
 function addBtn(){const t=tabs[cur];if(!t||t.b.length>=per)return;const a=acts[0];t.b.push({id:a.id,l:a.l.slice(0,12),d:a.d});sel=t.b.length-1;render();mark(true);say('Notun button: action bachai koro');$$('rmAct').focus()}
 const B=()=>tabs[cur]&&tabs[cur].b[sel];
 $$('rmAct').onchange=e=>{const x=B();if(!x)return;const v=e.target.value;
  if(PH[v]){x.id=v;x.d=false;if(!x.l||aid(kindOf(x.id))>=0)x.l={'keys:':'KEYS','url:':'LINK','app:':'APP'}[v]}
  else{const a=acts[aid(v)];const old=acts.find(q=>q.l.slice(0,12)===x.l);x.id=v;x.d=a.d;if(!x.l||old||PH[kindOf(x.id)]===undefined)x.l=a.l.slice(0,12)}
  render();mark(true)};
 $$('rmCustom').oninput=e=>{const x=B();if(!x)return;x.id=kindOf(x.id.startsWith('url:')?'url:':x.id.startsWith('app:')?'app:':'keys:')+e.target.value.trim();mark(true)};
 $$('rmAct').addEventListener('change',()=>{const x=B();if(x&&PH[$$('rmAct').value])$$('rmCustom').focus()});
 $$('rmLabel').oninput=e=>{const x=B();if(!x)return;x.l=e.target.value;renderGrid();mark(true)};
 $$('rmConfirm').onchange=e=>{const x=B();if(!x)return;x.d=e.target.checked;renderGrid();mark(true)};
 $$('rmName').oninput=e=>{const t=tabs[cur];if(!t)return;t.n=e.target.value.toUpperCase();renderTabs();renderGrid();mark(true)};
 $$('rmTabL').onclick=()=>{if(cur>0){[tabs[cur-1],tabs[cur]]=[tabs[cur],tabs[cur-1]];cur--;render();mark(true)}};
 $$('rmTabR').onclick=()=>{if(cur<tabs.length-1){[tabs[cur+1],tabs[cur]]=[tabs[cur],tabs[cur+1]];cur++;render();mark(true)}};
 $$('rmTabDel').onclick=()=>{if(tabs.length>1&&confirm('"'+tabs[cur].n+'" tab delete korbe?')){tabs.splice(cur,1);sel=-1;render();mark(true)}};
 $$('rmL').onclick=()=>{const b=tabs[cur].b;if(sel>0){[b[sel-1],b[sel]]=[b[sel],b[sel-1]];sel--;render();mark(true)}};
 $$('rmR').onclick=()=>{const b=tabs[cur].b;if(sel<b.length-1){[b[sel+1],b[sel]]=[b[sel],b[sel+1]];sel++;render();mark(true)}};
 $$('rmDel').onclick=()=>{tabs[cur].b.splice(sel,1);sel=-1;render();mark(true)};
 $$('rmPrevT').onclick=()=>{if(tabs.length>1){cur=(cur+tabs.length-1)%tabs.length;sel=-1;render()}};
 $$('rmNextT').onclick=()=>{if(tabs.length>1){cur=(cur+1)%tabs.length;sel=-1;render()}};
 $$('rmTest').onclick=async()=>{const x=B();if(!x)return;
  if(['keys:','url:','app:'].includes(x.id)){toast('Age value dao',true);return}
  const r=await post('/api/remote/send?c='+encodeURIComponent(x.id)+'&l='+encodeURIComponent(x.l));
  const ok=r&&r.ok;say(ok?x.l+' sent to PC':(r&&r.code===2?'Bad token':'PC agent not reachable'),!ok);toast(ok?x.l+' sent to PC':(r&&r.code===2?'Bad token':'PC agent not reachable (agent chalu? IP/port/token?)'),!ok)};
 $$('rmSave').onclick=async()=>{
  for(const t of tabs){if(!t.n.trim()){toast('Tab-er naam dao',true);return}
   for(const b of t.b){if(['keys:','url:','app:'].includes(b.id)){toast('"'+t.n+'" tab-e ekta custom button-e value nai',true);return}}}
  const r=await post('/api/settings',{rmt:{tabs:tabs.map(t=>({n:t.n.trim(),b:t.b.map(b=>({id:b.id,l:b.l,d:!!b.d}))}))}});
  toast(r&&r.ok?'Saved - CYD-te apply hoyeche':'Save failed',!(r&&r.ok));if(r&&r.ok){mark(false);loaded=false;loadState()}};
 $$('rmRevert').onclick=()=>{loaded=false;loadState()};
 $$('rmReset').onclick=()=>{if(!confirm('Default layout (6 tab) ferot nibe? Save na chaple CYD-te change hobe na.'))return;
  const D=[['MEDIA','playpause prev next stop mute volup voldown fullscreen space left'],['SYSTEM','lock desktop alttab taskview taskmgr screenshot minimize maximize monitor_off explorer'],['BROWSER','newtab closetab reopentab refresh back forward zoomin zoomout addrbar incognito'],['EDIT','copy cut paste undo redo selectall save find enter esc'],['SLIDES','slide_start slide_here blackscreen left right up down pgup pgdn esc'],['POWER','lock sleep restart shutdown cancel_shutdown closewin monitor_off']];
  tabs=D.map(d=>({n:d[0],b:d[1].split(' ').map(id=>{const a=acts[aid(id)];return{id,l:a.l.slice(0,12),d:a.d}})}));cur=0;sel=-1;render();mark(true)};
 const ff2=fillForms;
 fillForms=function(s){ff2(s);const r=s.rmt;if(!r)return;
  acts=r.acts;max=r.maxTabs||8;per=r.per||10;
  if(!$$('rmAct').options.length)fillActs();
  if(!loaded||!dirty){tabs=r.tabs.map(t=>({n:t.n,b:t.b.map(x=>({id:x.id,l:x.l,d:x.d}))}));loaded=true;if(!tabs.length)tabs=[{n:'MAIN',b:[]}];render();mark(false)}
  $$('rmtTokSt').textContent=r.tok?'(set)':'(not set)'};
 $$('rmtSave').addEventListener('click',async()=>{const t=$$('rmtTok'),b={};if(t.value.trim())b.token=t.value.trim();
  const x=await post('/api/settings',{rmt:b});toast(x&&x.ok?'Token saved':'Save failed',!(x&&x.ok));t.value='';loadState()});
 mark(false);
 window.addEventListener('beforeunload',e=>{if(dirty){e.preventDefault();e.returnValue=''}});
})();
</script></body></html>)UI";
