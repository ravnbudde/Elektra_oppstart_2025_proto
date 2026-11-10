@echo off
setlocal

set /p PORT=<port.txt
echo Using port: %PORT%

powershell -Command "(Get-Content 'docker-compose.yml') -replace '{{PORT}}', '%PORT%' | Set-Content 'docker-compose.yml'"

echo docker-compose.yml generated with port %PORT%
