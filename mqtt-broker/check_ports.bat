@echo off
set /p PORT=<port.txt
echo Checking if port %PORT% is in use...

set PID=

for /f "tokens=5" %%a in ('netstat -aon ^| find ":%PORT%" ^| find "LISTENING"') do (
    set PID=%%a
)

if defined PID (
    echo Port %PORT% is in use by PID: %PID%
    tasklist /svc /FI "PID eq %PID%"
) else (
    echo Port %PORT% is free.
)

pause
