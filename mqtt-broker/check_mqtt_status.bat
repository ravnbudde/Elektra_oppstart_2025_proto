@echo off
echo --- Sjekkar MQTT-status ---

REM 1. Sjekk om Docker Desktop er i gang
echo.
echo Kontrollerer Docker...
tasklist /FI "IMAGENAME eq Docker Desktop.exe" | find /I "Docker Desktop.exe" >nul
if %errorlevel%==0 (
    echo ✅ Docker Desktop køyrer.
) else (
    echo ❌ Docker Desktop er IKKJE i gang.
    echo Start Docker Desktop manuelt før du fortset.
    goto END
)

REM 2. Sjekk om Mosquitto-containeren køyrer
echo.
echo Kontrollerer Mosquitto-container...
docker ps --filter "name=mosquitto" --format "Status: {{.Status}}" | find /I "Up" >nul
if %errorlevel%==0 (
    echo Mosquitto-container er aktiv.
) else (
    echo Mosquitto-container er ikkje i gang.
    echo Bruk start_mqtt.bat for å starte den.
    goto END
)

REM 3. Vis lokal IP
echo.
echo Din lokale IP-adresse:
for /f "tokens=2 delims=:" %%a in ('ipconfig ^| findstr /c:"IPv4 Address"') do @echo    %%a

REM 4. Vis offentleg IP med PowerShell
echo.
echo Din offentlege IP-adresse:
powershell -Command "(Invoke-WebRequest -Uri 'https://api.ipify.org').Content"

:END
echo.
pause
