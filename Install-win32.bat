@echo off
md "%programfiles%\ONSlaught"
copy /y "bin-win32\ONSlaught.exe" "%programfiles%\ONSlaught"
copy /y Licence.txt "%programfiles%\ONSlaught"
copy /y bin-win32\*.dll "%programfiles%\ONSlaught"
rem bin-win32\vcredist_x86.exe
echo I will now try to add myself to PATH.
rem modifyPATH "%programfiles%\ONSlaught"
echo move /y *.dll "%systemroot%\system32" >"%programfiles%\ONSlaught\mov.bat"
echo mov.bat moves the DLLs to a shared location (%systemroot%\system32). >"%programfiles%\ONSlaught\readme.txt"
