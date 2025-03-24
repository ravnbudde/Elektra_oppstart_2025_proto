@echo off
call generate_compose.bat
echo Starting Mosquitto with port from port.txt...
docker compose up -d
pause
