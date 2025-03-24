@echo off
set /p USERNAME=Første brukarnamn (vil overskrive gamle brukarar!): 
docker run -it --rm -v "%cd%\config:/mosquitto/config" eclipse-mosquitto mosquitto_passwd -c /mosquitto/config/passwd %USERNAME%
pause
