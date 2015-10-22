@echo off
::Change this dir to your user folder. 
SET dir="C:\Users\AJ\git\3DS-Remote\"
::Change this to where your WinSCP is installed to. See Push.txt for details. 
SET winSCP="C:\Program Files (x86)\WinSCP"

ECHO Cleaning...
cd %dir%
make clean
ECHO Done! 
ECHO:

ECHO Building...
ECHO --Building-- > %dir%\make\make.log
make >> %dir%\make\make.log
if errorlevel 2 goto buildfail
ECHO Done! 
ECHO:

ECHO Pushing...
cd %winSCP%
ECHO --Pushing to remote server-- >> %dir%\make\make.log
winscp.com /script=%dir%make\Push.txt >> %dir%\make\make.log
if errorlevel 1 goto ftpfail
ECHO Done! 
pause
exit

:buildfail
ECHO Failed! Perhaps there is a compile error in your code? Check logs for details.
ECHO To view make's logs, see make.log in the output folder.
ECHO 
pause
exit

:ftpfail
ECHO Failed! Is your 3DS connected to the network with the FTP app loaded?
ECHO To view WinSCP's logs, see make.log in the output folder.
ECHO 
pause
exit