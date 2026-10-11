@echo off
rem CYD PC agent: monitor (/stats) + remote control (/cmd) in ONE program, ONE port.
rem Port 8080 is preferred; if it is busy the next free port is used and the CYD finds it by itself.
rem First run: allow it in the Windows Firewall prompt (Private networks).
cd /d "%~dp0"
where python >nul 2>nul || (echo Python 3 is not installed: https://www.python.org/downloads/ ^(tick "Add python to PATH"^) & pause & exit /b 1)
python -m pip install --quiet --disable-pip-version-check psutil
python pc_agent.py %*
pause
