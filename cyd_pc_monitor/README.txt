ESP32-2432S028R (CYD) PC Monitor
 - Open this folder in VS Code + PlatformIO, upload env "cyd" (ST7789, 2 USB ports).
 - Older single-USB board: env cyd_ili9341.  Screen black: env cyd_bl27.  Colours negative: color_invert. Red/blue swapped: color_bgr.
 - Touch raw values match the XPT2046_Touchscreen library (no flip). If everything is rotated 180 deg add -DTOUCH_INV_X -DTOUCH_INV_Y.
 - Touch uses a 5-point affine calibration (src/touch.h, touchToScreen()).

 - PC side = ONE file pair: run_pc_agent.bat + pc_agent.py (monitor /stats + remote /cmd, same port 8080).
   Port busy -> the agent takes the next free port and the CYD finds it by itself.
   Token (pc_agent_token.txt) -> CYD web panel -> Display tab -> PC remote -> Token.
 - The CYD Remote page header shows what the PC is doing ("PC: VOL +"), also for actions from other devices.
 - Touch: real XPT2046 library on VSPI -> SD card off in env cyd. Need the SD card? use env cyd_sd.

 - Remote page: any number of buttons (max 48, 12 per page). Swipe the buttons left/right or tap the < > arrows in the header to switch button pages;
   swipe in the header strip (or BOOT button) to leave the Remote page. Add / remove / move buttons in the web panel (Display tab -> PC remote).
   Custom buttons: keys:ctrl+shift+n  or  url:https://...   Dangerous ones (sleep/restart/shutdown/close) need a second tap (Confirm tick).
 - Every action (also from other devices) pops up on the CYD screen on every page except Media.
 - app:<name> launches any program/folder/file (notepad, chrome, C:\path\x.exe, spotify:).

 - REMOTE v3: tabs (MEDIA, SYSTEM, BROWSER, EDIT, SLIDES, POWER by default; up to 8) with 10 buttons each + PREV TAB / NEXT TAB buttons on every tab.
   Change tab: tap the NEXT/PREV buttons, tap the < > strip, or swipe the button grid. Leave the Remote page: swipe in the blue header or press BOOT.
   Popup = slim bar at the bottom (never covers buttons). Edit everything in the web panel -> "PC Remote" tab (visual preview, Save changes).
