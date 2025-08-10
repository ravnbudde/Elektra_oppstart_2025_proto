cd /d "%~dp0"

cd ..
cd ..

call ".venv\Scripts\activate.bat"
dir

python "src\maps\generate_map.py"

pause
