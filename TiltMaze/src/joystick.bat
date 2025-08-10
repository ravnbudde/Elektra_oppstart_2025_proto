cd /d "%~dp0"
cd ..
call ".venv\Scripts\activate.bat"

python "src\joystick.py"

pause
