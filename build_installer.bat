"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcpackages\vcbuild.exe" /M4 "Vc9S&C.sln" "Release|Win32"

@ECHO Get version
@ECHO off
DEL version.nsh
FOR /F "usebackq" %%s IN (`bin\steelandconcrete_server.exe --short_version`) DO @SET REV=%%s
ECHO !define VERSION "%REV%" >> version.nsh
@ECHO on

"C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
PAUSE