@echo off

REM === Sjekk om passwd-fila finst ===
IF NOT EXIST config\passwd (
    echo Ingen passordfil funne. Startar init_mqtt_passwd.bat...
    call init_mqtt_passwd.bat
)


call generate_compose.bat
echo Starting Mosquitto with port from port.txt...
docker compose up -d

echo.
echo Venter litt på at containeren skal starte...
timeout /t 2 >nul

echo Ventar til mosquitto.db eksisterer i containeren...

:wait_db
docker exec mosquitto sh -c "test -f /mosquitto/data/mosquitto.db" || (
    timeout /t 1 >nul
    goto wait_db
)

echo Fila eksisterer – set rettar...
docker exec mosquitto chmod 0700 /mosquitto/data/mosquitto.db


pause
