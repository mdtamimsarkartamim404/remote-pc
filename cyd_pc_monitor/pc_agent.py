#!/usr/bin/env python3
"""
pc_agent.py  -  ONE agent for the CYD:  PC monitor (GET /stats)  +  PC remote control (GET /cmd).

Run:     python pc_agent.py                 (or double-click run_pc_agent.bat)
         python pc_agent.py --port 8080     (preferred port; if it is busy the next free one is used)

* Same port for monitor and remote. If the port is busy the agent moves to the next free port and the
  CYD finds it again by itself (it scans the neighbouring ports when it loses the connection).
* Buttons can also launch apps (app:notepad, app:C:\\path\\x.exe, app:spotify:) or open links / press shortcuts.
* Remote token: created on first start in pc_agent_token.txt (an old pc_remote_token.txt is reused).
  Type it once in the CYD web panel (Display tab -> PC remote -> Token -> Save).
* The agent tells the CYD what it is doing ("act" in /stats) so the CYD Remote page shows
  "PC: VOL +" even when the button was pressed from the web panel or another device.
* Needs:  pip install psutil   (installed automatically on first start if possible).
Only run it on a network you trust: anyone with the token can press keys on this computer.
"""
import argparse, json, os, platform, secrets, socket, subprocess, sys, threading, time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import urlparse, parse_qs

try:
    import psutil
except ImportError:
    print("psutil missing -> installing ...")
    subprocess.call([sys.executable, "-m", "pip", "install", "--quiet", "psutil"])
    import psutil

OS = platform.system()
HERE = os.path.dirname(os.path.abspath(__file__))
TOKEN_FILE = os.path.join(HERE, "pc_agent_token.txt")
OLD_TOKEN_FILE = os.path.join(HERE, "pc_remote_token.txt")
NOWIN = 0x08000000 if OS == "Windows" else 0          # CREATE_NO_WINDOW for helper processes


def load_token():
    for f in (TOKEN_FILE, OLD_TOKEN_FILE):
        if os.path.exists(f):
            t = open(f, encoding="utf-8").read().strip()
            if t:
                if f != TOKEN_FILE:
                    open(TOKEN_FILE, "w", encoding="utf-8").write(t)
                return t
    t = secrets.token_hex(4)
    open(TOKEN_FILE, "w", encoding="utf-8").write(t)
    return t


TOKEN = load_token()

# =====================================================================  REMOTE (keys / power)
if OS == "Windows":
    import ctypes
    u32 = ctypes.windll.user32
    VK = {"ctrl": 0x11, "shift": 0x10, "alt": 0x12, "win": 0x5B, "tab": 0x09, "esc": 0x1B, "enter": 0x0D, "space": 0x20,
          "left": 0x25, "up": 0x26, "right": 0x27, "down": 0x28, "backspace": 0x08, "delete": 0x2E, "insert": 0x2D,
          "home": 0x24, "end": 0x23, "pgup": 0x21, "pgdn": 0x22, "plus": 0xBB, "minus": 0xBD, "equals": 0xBB, "period": 0xBE,
          "comma": 0xBC, "slash": 0xBF, "playpause": 0xB3, "next": 0xB0, "prev": 0xB1, "stop": 0xB2,
          "volup": 0xAF, "voldown": 0xAE, "mute": 0xAD}
    for _i in range(1, 13):
        VK["f%d" % _i] = 0x6F + _i
    for _c in "abcdefghijklmnopqrstuvwxyz0123456789":
        VK[_c] = ord(_c.upper())
    EXT = {0xB3, 0xB0, 0xB1, 0xB2, 0xAF, 0xAE, 0xAD, 0x5B, 0x25, 0x26, 0x27, 0x28, 0x21, 0x22, 0x23, 0x24, 0x2D, 0x2E}

    def press(*names):
        codes = [VK[n] for n in names]
        for c in codes:
            u32.keybd_event(c, 0, 1 if c in EXT else 0, 0)
        time.sleep(0.02)
        for c in reversed(codes):
            u32.keybd_event(c, 0, (1 if c in EXT else 0) | 2, 0)

    def monitor_off():
        u32.SendMessageW(0xFFFF, 0x112, 0xF170, 2)

    def lock():
        u32.LockWorkStation()

    def sleep_pc():
        subprocess.Popen(["rundll32.exe", "powrprof.dll,SetSuspendState", "0,1,0"])

    def shutdown_cmd(*a):
        subprocess.Popen(["shutdown", *a])
else:
    try:
        import pyautogui
        pyautogui.FAILSAFE = False
    except Exception:
        pyautogui = None
    KEYMAP = {"up": "up", "down": "down", "backspace": "backspace", "delete": "delete", "home": "home", "end": "end",
              "pgup": "pageup", "pgdn": "pagedown", "plus": "+", "minus": "-", "equals": "=", "period": ".", "comma": ",", "slash": "/",
              "playpause": "playpause", "next": "nexttrack", "prev": "prevtrack", "stop": "stop", "volup": "volumeup",
              "voldown": "volumedown", "mute": "volumemute", "tab": "tab", "esc": "esc", "enter": "enter", "space": "space",
              "left": "left", "right": "right", "f4": "f4", "f5": "f5", "ctrl": "ctrl", "shift": "shift", "alt": "alt",
              "win": "command" if OS == "Darwin" else "win", "d": "d", "s": "s", "c": "c", "v": "v", "z": "z"}

    def press(*names):
        if not pyautogui:
            raise RuntimeError("pip install pyautogui")
        pyautogui.hotkey(*[KEYMAP.get(n, n) for n in names])

    def monitor_off():
        subprocess.Popen(["pmset", "displaysleepnow"] if OS == "Darwin" else ["xset", "dpms", "force", "off"])

    def lock():
        subprocess.Popen(["pmset", "displaysleepnow"] if OS == "Darwin" else ["loginctl", "lock-session"])

    def sleep_pc():
        subprocess.Popen(["pmset", "sleepnow"] if OS == "Darwin" else ["systemctl", "suspend"])

    def shutdown_cmd(*a):
        if "/s" in a:
            subprocess.Popen(["shutdown", "-h", "now"] if OS == "Darwin" else ["systemctl", "poweroff"])
        elif "/r" in a:
            subprocess.Popen(["shutdown", "-r", "now"] if OS == "Darwin" else ["systemctl", "reboot"])

# id -> (label shown on the CYD popup, key combo "ctrl+shift+n")   - same ids as the CYD button catalogue
COMBOS = {
    "playpause": ("PLAY/PAUSE", "playpause"), "next": ("NEXT", "next"), "prev": ("PREV", "prev"), "stop": ("STOP", "stop"),
    "mute": ("MUTE", "mute"), "volup": ("VOL +", "volup+volup"), "voldown": ("VOL -", "voldown+voldown"),
    "desktop": ("DESKTOP", "win+d"), "alttab": ("ALT+TAB", "alt+tab"), "taskview": ("TASK VIEW", "win+tab"),
    "taskmgr": ("TASK MGR", "ctrl+shift+esc"), "screenshot": ("SNIP", "win+shift+s"),
    "minimize": ("MINIMIZE", "win+down"), "maximize": ("MAXIMIZE", "win+up"), "snap_left": ("SNAP LEFT", "win+left"),
    "snap_right": ("SNAP RIGHT", "win+right"), "run": ("RUN", "win+r"), "explorer": ("EXPLORER", "win+e"),
    "settings": ("SETTINGS", "win+i"), "search": ("SEARCH", "win+s"), "clipboard": ("CLIPBOARD", "win+v"),
    "emoji": ("EMOJI", "win+period"), "newdesk": ("NEW DESK", "win+ctrl+d"), "nextdesk": ("NEXT DESK", "win+ctrl+right"),
    "prevdesk": ("PREV DESK", "win+ctrl+left"), "gamebar": ("GAME BAR", "win+g"), "project": ("PROJECT", "win+p"),
    "notify": ("NOTIFY", "win+n"), "voicetype": ("VOICE TYPE", "win+h"), "closewin": ("CLOSE WIN", "alt+f4"),
    "copy": ("COPY", "ctrl+c"), "cut": ("CUT", "ctrl+x"), "paste": ("PASTE", "ctrl+v"), "undo": ("UNDO", "ctrl+z"),
    "redo": ("REDO", "ctrl+y"), "selectall": ("SELECT ALL", "ctrl+a"), "save": ("SAVE", "ctrl+s"), "find": ("FIND", "ctrl+f"),
    "newtab": ("NEW TAB", "ctrl+t"), "closetab": ("CLOSE TAB", "ctrl+w"), "reopentab": ("REOPEN TAB", "ctrl+shift+t"),
    "nexttab": ("NEXT TAB", "ctrl+tab"), "prevtab": ("PREV TAB", "ctrl+shift+tab"), "refresh": ("REFRESH", "f5"),
    "fullscreen": ("FULLSCREEN", "f11"), "back": ("BACK", "alt+left"), "forward": ("FORWARD", "alt+right"),
    "zoomin": ("ZOOM +", "ctrl+plus"), "zoomout": ("ZOOM -", "ctrl+minus"), "zoomreset": ("ZOOM 100%", "ctrl+0"),
    "addrbar": ("ADDRESS BAR", "ctrl+l"), "incognito": ("INCOGNITO", "ctrl+shift+n"),
    "up": ("UP", "up"), "down": ("DOWN", "down"), "left": ("LEFT", "left"), "right": ("RIGHT", "right"), "esc": ("ESC", "esc"),
    "enter": ("ENTER", "enter"), "space": ("SPACE", "space"), "tab": ("TAB", "tab"), "backspace": ("BACKSPACE", "backspace"),
    "delete": ("DELETE", "delete"), "pgup": ("PAGE UP", "pgup"), "pgdn": ("PAGE DOWN", "pgdn"), "home": ("HOME", "home"),
    "end": ("END", "end"), "slide_start": ("SLIDES F5", "f5"), "slide_here": ("SLIDE HERE", "shift+f5"),
    "blackscreen": ("BLACK", "b"),
}
SPECIAL = {
    "lock": ("LOCK PC", lambda: lock()), "monitor_off": ("SCREEN OFF", lambda: monitor_off()),
    "sleep": ("SLEEP", lambda: sleep_pc()),
    "restart": ("RESTART in 5s", lambda: shutdown_cmd("/r", "/t", "5")),
    "shutdown": ("SHUTDOWN in 5s", lambda: shutdown_cmd("/s", "/t", "5")),
    "cancel_shutdown": ("CANCEL OFF", lambda: shutdown_cmd("/a")),
}


def do_combo(combo):
    """'ctrl+shift+n' / 'volup+volup' -> key presses.  'x+x' (same key twice) = press it twice."""
    names = [n.strip().lower() for n in combo.split("+") if n.strip()]
    if not names:
        raise ValueError("empty key combo")
    if len(names) == 2 and names[0] == names[1]:
        press(names[0]); press(names[1]); return
    press(*names)


def launch_app(target):
    """Open any program / file / folder / URI the way the Start menu / Run box would:
         notepad   calc   chrome   spotify:   ms-settings:   C:\\Games\\app.exe   D:\\Movies   (Linux/macOS: command or app name)
       Quotes and shell characters are rejected, so a button cannot smuggle in extra commands."""
    if not target or any(c in target for c in '&|<>^%"\r\n`$;'):
        raise ValueError("bad app name")
    if OS == "Windows":
        # the same lookup as Win+R: PATH, App Paths (chrome, spotify ...), full paths, URI schemes
        os.startfile(target)
    elif OS == "Darwin":
        subprocess.Popen(["open", "-a", target] if not os.path.exists(target) else ["open", target])
    else:
        import shutil
        exe = shutil.which(target)
        subprocess.Popen([exe] if exe else ["xdg-open", target])


def resolve(cid):
    """action id -> (label, function).  Supports catalogue ids, keys:<combo> and url:<http(s) link>."""
    if cid in SPECIAL:
        return SPECIAL[cid]
    if cid in COMBOS:
        lab, combo = COMBOS[cid]
        return lab, (lambda: do_combo(combo))
    if cid.startswith("keys:"):
        combo = cid[5:]
        return combo.upper()[:20], (lambda: do_combo(combo))
    if cid.startswith("url:"):
        url = cid[4:].strip()
        if not url.lower().startswith(("http://", "https://")):
            raise ValueError("url must start with http:// or https://")
        import webbrowser
        return "OPEN LINK", (lambda: webbrowser.open(url))
    if cid.startswith("app:"):
        target = cid[4:].strip()
        return ("OPEN " + os.path.basename(target.rstrip("\\/")))[:20].upper(), (lambda: launch_app(target))
    raise KeyError(cid)


# kept for reference / other tools
ACTIONS = {k: (v[0], None) for k, v in list(COMBOS.items()) + list(SPECIAL.items())}

LAST_ACT = {"text": "", "t": 0.0, "seq": 0}


def set_act(text):
    LAST_ACT["text"], LAST_ACT["t"] = text, time.time()
    LAST_ACT["seq"] += 1          # the CYD shows a popup whenever this number changes


# =====================================================================  MONITOR (stats)
STATS = {}
LOCK = threading.Lock()


def rd_cpu_name():
    try:
        if OS == "Windows":
            import winreg
            k = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DESCRIPTION\System\CentralProcessor\0")
            return winreg.QueryValueEx(k, "ProcessorNameString")[0].strip()
        if OS == "Linux":
            for l in open("/proc/cpuinfo"):
                if l.startswith("model name"):
                    return l.split(":", 1)[1].strip()
    except Exception:
        pass
    return platform.processor() or platform.machine()


def lan_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("10.255.255.255", 1))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return ""


def active_window():
    """(window title, process name) - Windows only."""
    if OS != "Windows":
        return "", ""
    try:
        import ctypes
        from ctypes import wintypes
        h = u32.GetForegroundWindow()
        n = u32.GetWindowTextLengthW(h)
        buf = ctypes.create_unicode_buffer(n + 1)
        u32.GetWindowTextW(h, buf, n + 1)
        pid = wintypes.DWORD()
        u32.GetWindowThreadProcessId(h, ctypes.byref(pid))
        app = psutil.Process(pid.value).name() if pid.value else ""
        return buf.value, app
    except Exception:
        return "", ""


def ascii_(s, n):
    return s.encode("ascii", "replace").decode()[:n]


def cpu_temp():
    try:
        t = psutil.sensors_temperatures()
        for key in ("coretemp", "k10temp", "cpu_thermal", "zenpower", "acpitz"):
            if key in t and t[key]:
                return max(x.current for x in t[key])
        for v in t.values():
            if v:
                return v[0].current
    except Exception:
        pass
    if OS == "Windows":                 # ACPI thermal zone (works on many laptops, sometimes needs admin)
        try:
            out = subprocess.check_output(
                ["powershell", "-NoProfile", "-Command",
                 "(Get-CimInstance -Namespace root/wmi -ClassName MSAcpi_ThermalZoneTemperature | "
                 "Select-Object -First 1 -ExpandProperty CurrentTemperature)"],
                timeout=6, creationflags=NOWIN).decode().strip()
            if out:
                return int(out) / 10.0 - 273.15
        except Exception:
            pass
    return -1


def gpu_info():
    try:
        out = subprocess.check_output(
            ["nvidia-smi", "--query-gpu=utilization.gpu,temperature.gpu,memory.used,memory.total",
             "--format=csv,noheader,nounits"], timeout=3, creationflags=NOWIN).decode().splitlines()[0]
        u, t, mu, mt = [float(x) for x in out.split(",")]
        return u, t, mu / 1024.0, mt / 1024.0
    except Exception:
        return -1, -1, 0, 0


def ping_ms():
    for host in (("1.1.1.1", 443), ("8.8.8.8", 53)):
        try:
            t = time.time()
            socket.create_connection(host, 1.0).close()
            return (time.time() - t) * 1000.0
        except Exception:
            pass
    return -1


def slow_loop():
    """temperature / gpu / ping / processes: things that are slower to read, refreshed every few seconds."""
    procs = {}
    last_io = {}
    t_prev = time.time()
    n = 0
    while True:
        extra = {}
        try:
            if n % 3 == 0:
                extra["temp"] = cpu_temp()
            u, t, mu, mt = gpu_info()
            extra.update(gpu=u, gpu_temp=t, gpu_mem_used=mu, gpu_mem_total=mt)
            if n % 2 == 0:
                extra["ping"] = ping_ms()
            now = time.time()
            dt = max(now - t_prev, 0.5)
            t_prev = now
            mem, io, rows, seen = {}, {}, [], set()
            for p in psutil.process_iter(["pid", "name", "memory_info"]):
                try:
                    pid = p.info["pid"]
                    seen.add(pid)
                    nm = (p.info["name"] or "?")
                    if nm.lower().endswith(".exe"):
                        nm = nm[:-4]
                    pr = procs.get(pid)
                    if pr is None:
                        pr = procs[pid] = p
                        pr.cpu_percent(None)
                        c = 0.0
                    else:
                        c = pr.cpu_percent(None) / (psutil.cpu_count() or 1)
                    m = p.info["memory_info"].rss / 1048576.0 if p.info["memory_info"] else 0
                    mem[nm] = mem.get(nm, 0) + m
                    try:
                        ic = pr.io_counters()
                        tot = ic.read_bytes + ic.write_bytes
                        prev = last_io.get(pid, tot)
                        last_io[pid] = tot
                        io[nm] = io.get(nm, 0) + (tot - prev) / 1048576.0 / dt
                    except Exception:
                        pass
                    rows.append((nm, c, m))
                except Exception:
                    pass
            for d in (procs, last_io):
                for pid in [k for k in d if k not in seen]:
                    d.pop(pid, None)
            agg = {}
            for nm, c, m in rows:
                a = agg.setdefault(nm, [0.0, 0.0])
                a[0] += c
                a[1] += m
            top = sorted(agg.items(), key=lambda kv: kv[1][0], reverse=True)[:8]
            extra["procs"] = [{"n": ascii_(k, 15), "c": round(v[0], 1), "m": int(v[1])} for k, v in top]
            extra["rprocs"] = [{"n": ascii_(k, 15), "m": int(v)} for k, v in
                               sorted(mem.items(), key=lambda kv: kv[1], reverse=True)[:4]]
            extra["dprocs"] = [{"n": ascii_(k, 15), "r": round(v, 2)} for k, v in
                               sorted(io.items(), key=lambda kv: kv[1], reverse=True)[:4] if v > 0.01]
            extra["nproc"] = len(seen)
        except Exception as e:
            print("slow_loop:", e)
        with LOCK:
            STATS.update(extra)
        n += 1
        time.sleep(2.0)


def fast_loop():
    host = socket.gethostname()[:23]
    name = ascii_(rd_cpu_name(), 39)
    ip = lan_ip()
    psutil.cpu_percent(None, percpu=True)
    nio, dio, t0 = psutil.net_io_counters(), psutil.disk_io_counters(), time.time()
    n = 0
    while True:
        time.sleep(1.0)
        try:
            now = time.time()
            dt = max(now - t0, 0.2)
            t0 = now
            n2, d2 = psutil.net_io_counters(), psutil.disk_io_counters()
            up = (n2.bytes_sent - nio.bytes_sent) / 1024.0 / dt
            down = (n2.bytes_recv - nio.bytes_recv) / 1024.0 / dt
            dr = (d2.read_bytes - dio.read_bytes) / 1048576.0 / dt if d2 and dio else 0
            dw = (d2.write_bytes - dio.write_bytes) / 1048576.0 / dt if d2 and dio else 0
            nio, dio = n2, d2
            cores = psutil.cpu_percent(None, percpu=True)
            vm, sw = psutil.virtual_memory(), psutil.swap_memory()
            fr = psutil.cpu_freq()
            drives = []
            for pt in psutil.disk_partitions(all=False):
                if len(drives) >= 4:
                    break
                if OS == "Windows" and ("cdrom" in pt.opts or not pt.fstype):
                    continue
                try:
                    us = psutil.disk_usage(pt.mountpoint)
                except Exception:
                    continue
                label = pt.mountpoint.rstrip("\\/") or pt.mountpoint
                drives.append({"n": ascii_(label if OS == "Windows" else (os.path.basename(label) or "/"), 7),
                               "p": round(us.percent, 1), "u": round(us.used / 2**30, 1), "t": round(us.total / 2**30, 1)})
            sysdisk = drives[0]["p"] if drives else 0
            bat = psutil.sensors_battery()
            win, wapp = active_window()
            lt = time.localtime()
            d = {
                "host": host, "name": name, "cpu": round(sum(cores) / max(len(cores), 1), 1), "cores": [round(c) for c in cores],
                "freq": round(fr.current) if fr else 0, "ram": vm.percent, "ram_used": round(vm.used / 2**30, 1),
                "ram_total": round(vm.total / 2**30, 1), "disk": sysdisk, "up": round(up, 1), "down": round(down, 1),
                "uptime": int(now - psutil.boot_time()), "swap": sw.percent, "dr": round(dr, 2), "dw": round(dw, 2),
                "tx_gb": round(n2.bytes_sent / 2**30, 2), "rx_gb": round(n2.bytes_recv / 2**30, 2), "ip": ip,
                "bat": int(bat.percent) if bat else -1, "plug": bool(bat.power_plugged) if bat else False,
                "win": ascii_(win, 51), "wapp": ascii_(wapp, 23), "drives": drives,
                "time": time.strftime("%H:%M:%S", lt), "date": time.strftime("%a %d %b %Y", lt),
            }
            with LOCK:
                STATS.update(d)
        except Exception as e:
            print("fast_loop:", e)
        n += 1


def stats_json():
    with LOCK:
        d = dict(STATS)
    d.setdefault("temp", -1)
    d.setdefault("gpu", -1)
    age = time.time() - LAST_ACT["t"]
    d["act"] = ascii_(LAST_ACT["text"], 27) if LAST_ACT["text"] and age < 6 else ""
    d["seq"] = LAST_ACT["seq"]
    return json.dumps(d)


# =====================================================================  HTTP
class H(BaseHTTPRequestHandler):
    def log_message(self, fmt, *a):
        pass

    def reply(self, code, text, ctype="text/plain"):
        b = text.encode()
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(b)))
        self.send_header("Connection", "close")
        self.end_headers()
        self.wfile.write(b)

    def do_GET(self):
        u = urlparse(self.path)
        q = parse_qs(u.query)
        if u.path == "/ping":
            return self.reply(200, "pc_agent ok")
        if u.path == "/stats":
            return self.reply(200, stats_json(), "application/json")
        if u.path != "/cmd":
            return self.reply(404, "not found")
        who = self.client_address[0]
        if q.get("k", [""])[0] != TOKEN:
            log("%s  DENIED (bad token) from %s" % (time.strftime("%H:%M:%S"), who))
            set_act("BAD TOKEN")
            return self.reply(403, "bad token")
        c = q.get("c", [""])[0]
        try:
            label, fn = resolve(c)
        except KeyError:
            return self.reply(400, "unknown action")
        except Exception as e:
            set_act("BAD: %s" % e)
            return self.reply(400, "error: %s" % e)
        custom = q.get("l", [""])[0].strip()
        if custom:
            label = custom
        set_act(label)
        log("%s  %-16s from %s" % (time.strftime("%H:%M:%S"), label, who))
        try:
            fn()
            self.reply(200, "ok " + c)
        except Exception as e:
            set_act("FAILED: " + label)
            log("            error: %s" % e)
            self.reply(500, "error: %s" % e)


def log(s):
    sys.stdout.write(s + "\n")
    sys.stdout.flush()


def is_our_agent(port):
    try:
        s = socket.create_connection(("127.0.0.1", port), 0.5)
        s.sendall(b"GET /ping HTTP/1.0\r\n\r\n")
        data = s.recv(200)
        s.close()
        return b"pc_agent" in data
    except Exception:
        return False


def lan_ips():
    ips = {lan_ip()}
    try:
        for i in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            ips.add(i[4][0])
    except Exception:
        pass
    return sorted(i for i in ips if i and not i.startswith("127."))


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8080, help="preferred port (default 8080)")
    a = ap.parse_args()

    srv, port = None, a.port
    for p in range(a.port, a.port + 10):
        try:
            ThreadingHTTPServer.allow_reuse_address = False
            srv = ThreadingHTTPServer(("0.0.0.0", p), H)
            port = p
            break
        except OSError:
            if is_our_agent(p):
                print("pc_agent is already running on port %d - nothing to do." % p)
                sys.exit(0)
            print("port %d is busy -> trying %d" % (p, p + 1))
    if srv is None:
        print("no free port in %d..%d" % (a.port, a.port + 9))
        sys.exit(1)

    threading.Thread(target=fast_loop, daemon=True).start()
    threading.Thread(target=slow_loop, daemon=True).start()
    print("=" * 62)
    print(" CYD PC agent running  (monitor + remote, one port)")
    print(" Port  :", port, "" if port == a.port else "  (preferred %d was busy; the CYD will find this port by itself)" % a.port)
    print(" IP    :", ", ".join(lan_ips()) or "?")
    print(" Token :", TOKEN, "   (file: %s)" % TOKEN_FILE)
    print(" CYD web panel -> PC IP + Port, and Display tab -> PC remote -> Token")
    print("=" * 62)
    print(" Actions pressed on the CYD appear below and on the CYD screen:")
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        pass
