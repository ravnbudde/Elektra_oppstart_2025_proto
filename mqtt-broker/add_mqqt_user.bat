@echo off
set /p USERNAME=Brukarnamn: 
docker run -it --rm -v "%cd%\config:/mosquitto/config" eclipse-mosquitto mosquitto_passwd /mosquitto/config/passwd %USERNAME%
pause
